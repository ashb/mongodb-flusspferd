
#include "mongodb.hpp"
#include "collection.hpp"
#include <flusspferd.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace flusspferd;
using boost::lexical_cast;

namespace  mongodb_flusspferd {


FLUSSPFERD_LOADER(container, context) {
  // Ensure the binary module is loaded
  context.call("require", "binary");

  load_class<mongo_client>(container);
}

mongo_client::mongo_client(object const &obj, call_context &x)
  : base_type(obj)
{
  if (x.arg.size() == 0) {
      throw exception ("Mongo requires more than 0 arguments");
  }
  string dsn = x.arg[0];

  connection_.connect(dsn.c_str());
}

mongo_client::~mongo_client() { }


void mongo_client::close() { }

namespace {
  // Work out what kind of value `v` is and add it to the BSON (build) under a name of `key`
  void append_bson_field(mongo::BSONObjBuilder &b, char const *key, value const &v) {

    if (v.is_undefined()){
      b.appendUndefined(key);
    }
    else if (v.is_int()) {
      b.append(key, v.get_int());
    } else if (v.is_double()) {
      b.append(key, v.get_double());
    } else if (v.is_null()) {
      b.appendNull(key);
    } else if ( v.is_object() ) {

      object o = v.get_object();

      // A blob
      if (is_native<binary>(o) ) {
        binary &blob = flusspferd::get_native<binary>(v.get_object());        
        binary::vector_type const &vec = blob.get_const_data();

        b.appendBinData(
          key,
          vec.size(),
          mongo::ByteArray,
          (vec.empty() ? 0 : &vec[0])
        );
      }
      else if (o.is_array()) {
        array a = o;
        b.appendArray(key, mongo_client::array_to_bson( a ));
      }
      // Check if the object is a Date or a RegExp
      // TODO: This if would be better as instance_of. We dont have it in flusspferd 0.8 though :(
      else if (global().get_property_object("Date")
                       .get_property_object("prototype")
                       .call("isPrototypeOf", v)
                       .to_boolean()) 
      {

        unsigned long long d = o.call("valueOf").to_integral_number(64, false);
        b.appendDate(key, d);
      }
      else if (global().get_property_object("RegExp")
                       .get_property_object("prototype")
                       .call("isPrototypeOf", v)
                       .to_boolean()) 
      {
        std::string flags;

        if (o.get_property("global").to_boolean())
          flags += "g";
        else if (o.get_property("ignoreCase").to_boolean())
          flags += "i";
        else if (o.get_property("multiline").to_boolean())
          flags += "m";

        b.appendRegex(key, o.get_property("source").to_std_string(), flags);
      }
      else {
        // TODO: Maybe this should recurse and create sub objects
        b.append(key, v.to_std_string());
      }
    } else {
      // Default, stringify the object
      b.append(key, v.to_std_string());
    }
  }

  value bson_ele_to_object(mongo::BSONElement e) {
    mongo::BSONType type = e.type();

    using namespace mongo;
    switch (type) {
      case String:
        return value(e.str());

      case Object:
        return mongo_client::bson_to_object(e.embeddedObject());

      case Array:
        return mongo_client::bson_to_array(e);

      case BinData:
      {
        int len = 0;
        unsigned char const *data = reinterpret_cast<unsigned char const*>(e.binData(len));

        return create_native_object<byte_string>(object(), data, len);
      }
      case Undefined:
        return value();

      case Bool:
        return value(e.boolean());

      case Date:
      {
        unsigned long long d = e.date();

        // TODO: Is there any way that isn't so truely horrible?
        std::string js = "new Date(";
        return evaluate(js + boost::lexical_cast<std::string>(d) + ")");
      }
      case jstNULL:
        return object();

      case RegEx:
        return global().call("RegExp", e.regex(), e.regexFlags());

      case NumberInt:
      case NumberDouble:
        return value(e.number());

      case jstOID:
        return value(e.__oid().str());
      default:
        boost::format f("Unknown BSONElement type %1%");
        throw flusspferd::exception( boost::str( f % type ) );
        break;
    }

  }
}

mongo::BSONObj mongo_client::array_to_bson(array arr) {
  mongo::BSONObjBuilder b;

  std::size_t len = arr.size();
  for (std::size_t n = 0; n < len; ++n) {
    append_bson_field(b, lexical_cast<std::string>(n).c_str(), arr.get_element(n));
  }
  return b.obj();
}

mongo::BSONObj mongo_client::object_to_bson(object obj) {
  mongo::BSONObjBuilder b;

  for (property_iterator it = obj.begin(); it != obj.end(); ++it) {

    append_bson_field(b, it->to_string().c_str(), obj.get_property(*it));
  }

  return b.obj();
}

object mongo_client::bson_to_object(mongo::BSONObj bson) {
  std::set<std::string> fields;
  bson.getFieldNames(fields);

  object o = create_object();
  BOOST_FOREACH( std::string const &key, fields) {
    o.set_property(key,  bson_ele_to_object(bson.getField(key)));
  }

  return o;
}

array mongo_client::bson_to_array(mongo::BSONElement e) {
  mongo::BSONObj bson = e.embeddedObject();

  std::set<std::string> fields;
  bson.getFieldNames(fields);

  array a = create_array();
  BOOST_FOREACH( std::string const &key, fields) {
    value v = bson_ele_to_object(bson.getField(key));
    try {
      a.set_element(lexical_cast<size_t>(key), v);
    }
    catch (boost::bad_lexical_cast) {
      a.set_property(key, v);
    }
  }

  return a;
}

object mongo_client::find(string ns, object query, object fields, int limit, int size) {

  mongo::BSONObj bson = object_to_bson(query);
}

void mongo_client::insert(string ns, object obj) {
  mongo::BSONObj bson = object_to_bson(obj);
  connection_.insert(ns.c_str(), bson);
}

void mongo_client::update(string ns, object query, object obj, boost::optional<bool> upsert_) {
  // WTF is upsert? its in the mongo C++ api
}

void mongo_client::remote(string ns, object query, boost::optional<bool> just_one_) {
}

} // end anon namespace

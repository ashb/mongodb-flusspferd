
#include "mongodb.hpp"
#include "cursor.hpp"
#include <flusspferd.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>

using namespace flusspferd;
using boost::lexical_cast;
using boost::optional;
using boost::format;
namespace bf = boost::fusion;
using mongo::BSONObj;

namespace  mongodb_flusspferd {

static std::string get_ns(value &v) {
  if (v.is_undefined_or_null())
    throw exception("namespace (first argument) cannot be null or undefined");

  string s = v.to_string();
  return s.to_string();
}

static const format oid_to_str_fmt_("MongoDB.OID('%1%')");
static const format oid_to_source_fmt_("require('mongodb').OID('%1%')");

FLUSSPFERD_CLASS_DESCRIPTION(
  mongo_oid,
  (full_name, "MongoDB.OID")
  (constructor_name, "OID")
  (methods,
    ("toString", bind, to_string)
    ("toSource", bind, to_source)
    ("valueOf", bind, value_of)
  )
) {
protected:
  mongo::OID oid_;

public:

  // C++/native ctor
  mongo_oid(object const &obj, mongo::OID oid)
    : base_type(obj),
      oid_(oid)
  { }

  // Javascript ctor
  mongo_oid(object const &obj, call_context &x)
    : base_type(obj)
  {
    if (x.arg.size() == 0) {
      oid_.init();
    }
    else {
      std::string const &s = x.arg[0].to_std_string();
      oid_.init(s);
    }
  }

  ~mongo_oid() { }

  string to_string() {
    return boost::str( format(oid_to_str_fmt_) % oid_.str() );
  }

  string to_source() {
    return boost::str( format(oid_to_source_fmt_) % oid_.str() );
  }

  string value_of() {
    return oid_.str();
  }

  mongo::OID const *get_oid() {
    return &oid_;
  }
};


FLUSSPFERD_LOADER(container, context) {
  // Ensure the binary module is loaded
  context.call("require", "binary");

  object ctor = load_class<mongo_client>(container);
  load_class<mongo_oid>(container);
  load_class<cursor>(container);
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
        binary &blob = flusspferd::get_native<binary>(o);
        binary::vector_type const &vec = blob.get_const_data();

        b.appendBinData(
          key,
          vec.size(),
          mongo::ByteArray,
          (vec.empty() ? 0 : &vec[0])
        );
      }
      else if (is_native<mongo_oid>(o)) {
        mongo_oid &oid = flusspferd::get_native<mongo_oid>(o);
        b.appendOID(key, const_cast<mongo::OID*>( oid.get_oid() ) );
      }
      else if (o.is_array()) {
        array a = o;
        b.appendArray(key, mongo_client::array_to_bson( a ));
      }
      // Check if the object is a Date or a RegExp
      else if (o.instance_of(global().get_property_object("Date")))
      {

        unsigned long long d = o.call("valueOf").to_integral_number(64, false);
        b.appendDate(key, d);
      }
      else if (o.instance_of(global().get_property_object("RegExp")))
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
        b.append(key, mongo_client::object_to_bson(o));
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

        return create<byte_string>(bf::make_vector(data, len));
      }
      case Undefined:
        return value();

      case Bool:
        return value(e.boolean());

      case Date:
      {
        unsigned long long d = e.date();

        // TODO: Is there any way that isn't so truely horrible? Not yet.
        //       https://bugzilla.mozilla.org/show_bug.cgi?id=480850
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
        return create<mongo_oid>(bf::make_vector(e.__oid()));
      default:
        throw flusspferd::exception( format("Unknown BSONElement type %1%") % type );
        break;
    }

  }
}

BSONObj mongo_client::array_to_bson(array arr) {
  mongo::BSONObjBuilder b;

  std::size_t len = arr.size();
  for (std::size_t n = 0; n < len; ++n) {
    append_bson_field(b, lexical_cast<std::string>(n).c_str(), arr.get_element(n));
  }
  return b.obj();
}

BSONObj mongo_client::object_to_bson(object obj) {
  if (obj.is_null())
    return BSONObj();

  mongo::BSONObjBuilder b;

  for (property_iterator it = obj.begin(); it != obj.end(); ++it) {

    append_bson_field(b, it->to_string().c_str(), obj.get_property(*it));
  }

  return b.obj();
}

object mongo_client::bson_to_object(BSONObj bson) {

  root_object o(create<object>());

  mongo::BSONObjIterator it(bson);
  while ( it.more() ) {
    mongo::BSONElement e = it.next();

    o.set_property(e.fieldName(), bson_ele_to_object(e));
  }

  return o;
}

array mongo_client::bson_to_array(mongo::BSONElement e) {
  BSONObj bson = e.embeddedObject();

  mongo::BSONObjIterator it(bson);

  root_array a(create<array>());
  while ( it.more() ) {
    mongo::BSONElement e = it.next();
    value v = bson_ele_to_object(e);
    try {
      a.set_element(lexical_cast<size_t>(e.fieldName()), v);
    }
    catch (boost::bad_lexical_cast) {
      a.set_property(e.fieldName(), v);
    }
  }

  return a;
}


object mongo_client::find(value ns_, object query,
    optional<int> limit, optional<int> skip, optional<object> fields)
{
  std::string const &ns = get_ns(ns_);
  BSONObj field_bson,query_bson = object_to_bson(query);

  if (fields)
    field_bson = object_to_bson(*fields);

  boost::shared_ptr<mongo::DBClientCursor> cursor_ptr(connection_.query(
    ns,
    query_bson,
    limit.get_value_or(0),
    skip.get_value_or(0),
    // Empty object
    fields ? &field_bson : 0
  ));

  if (!cursor_ptr)
    return object();

  return create<cursor>(bf::make_vector(cursor_ptr));
}

object mongo_client::find_one(value ns_, object query,
    optional<object> fields)
{
  std::string const &ns = get_ns(ns_);
  BSONObj field_bson,query_bson = object_to_bson(query);

  if (fields)
    field_bson = object_to_bson(*fields);

  BSONObj o = connection_.findOne(
    ns,
    query_bson,
    fields ? &field_bson : 0
  );

  // Will return an empty bson object if nothing found
  if (o.firstElement().eoo())
    return object();

  return bson_to_object(o);
}


void mongo_client::insert(value ns_, object obj) {
  std::string const &ns = get_ns(ns_);
  connection_.insert(ns, object_to_bson(obj));
}

void mongo_client::update(value ns_, object query,
    object obj, optional<bool> upsert, optional<bool> multi)
{
  std::string const &ns = get_ns(ns_);

  connection_.update(
    ns,
    object_to_bson(query),
    object_to_bson(obj),
    upsert.get_value_or(false),
    multi.get_value_or(false)
  );
}

void mongo_client::remove(value ns_, object query,
    optional<bool> just_one_)
{
  std::string const &ns = get_ns(ns_);
  connection_.remove(ns, object_to_bson(query), just_one_.get_value_or(false));
}

object mongo_client::get_dbs() {
  std::list<std::string> dbs = connection_.getDatabaseNames();
  return create<array>(dbs);
}

object mongo_client::get_collections(value ns_) {
  std::string const &ns = get_ns(ns_);
  std::list<std::string> cols = connection_.getCollectionNames(ns);
  return create<array>(cols);
}

bool mongo_client::exists(value ns_) {
  std::string const &ns = get_ns(ns_);
  return connection_.exists(ns);
}

object mongo_client::run_cmd(value db_, object cmd) {
  std::string const &db = get_ns(db_);
  BSONObj out;
  connection_.runCommand(db, object_to_bson(cmd), out);
  return bson_to_object(out);
}

} // namespace mongodb_flusspferd

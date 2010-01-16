
#include "cursor.hpp"
#include "mongodb.hpp"

using namespace flusspferd;
using namespace mongodb_flusspferd;
using mongo::BSONObj;
using boost::optional;

cursor::cursor(object const &obj, mongo_client *db, string ns,
  BSONObj query, optional<BSONObj> fields)
  : base_type(obj),
    db_(*db),
    // Get a copy of the chars rather than rooting
    ns_(ns),
    query_(query),
    fields_(fields),
    limit_(0),
    skip_(0)
{
}

cursor::~cursor() {}

void cursor::trace( tracer &trc ) {
  trc("MongoDB.Cursor#db_", db_);
  trc("MongoDB.Cursor#ns_", ns_);
}

void cursor::assert_no_cursor() {
  if (cursor_)
    throw exception("MongoDB.Cursor: query is read only after results have been fetched");
}

boost::shared_ptr<mongo::DBClientCursor> cursor::mongo_cursor() {

  if ( !cursor_ ) {
    cursor_ = db_.connection_.query(
      ns_.to_string(), query_, limit_, skip_, fields_.get_ptr()
    )
    ;
  }

  return cursor_;
}

int cursor::get_id() {
  return mongo_cursor()->getCursorId();
}

bool cursor::more() {
  return mongo_cursor()->more();
}

object cursor::next() {
  if (!mongo_cursor()->more())
    // return null
    return object();

  mongo::BSONObj n = cursor_->next();
  mongo::BSONElement e = n.firstElement();

  root_object o(mongo_client::bson_to_object( n ));

  if (strcmp(e.fieldName(), "$err") == 0) {
    throw exception(o.get_property("$err").to_string());
  }

  return o;
}

object cursor::explain() {
  return object();
}

object cursor::hint() {
  assert_no_cursor();
  return object();
}

object cursor::min(object key) {
  assert_no_cursor();
  query_.minKey( mongo_client::object_to_bson( key ) );
  return *this;
}

object cursor::max(object key) {
  assert_no_cursor();
  query_.maxKey( mongo_client::object_to_bson( key ) );
  return *this;
}

object cursor::snapshot() {
  return object();
}

void cursor::sort(call_context &x) {
 //  MongoDB.Cursor#sort(field[, asc=1]) -> MongoDB.Cursor
    //  MongoDB.Cursor#sort(sortPattern) -> MongoDB.Cursor

  if (x.arg.size() == 1) {
    if (x.arg[0].is_object()) {
      object sort = x.arg[0].get_object();

      query_.sort( mongo_client::object_to_bson( sort ) );
    }
    else {
      query_.sort(x.arg[0].to_std_string());
    }
  }
  else if (x.arg.size() >= 2) {
    query_.sort(x.arg[0].to_std_string(), x.arg[1].to_number());
  }
  else
    throw exception("MongoDB.Cursor#sort needs at least one argument", "TypeError");

  x.result = x.self;
}

object cursor::where() {
  assert_no_cursor();
  return object();
}



#include "cursor.hpp"
#include "mongodb.hpp"

using namespace flusspferd;
using namespace mongodb_flusspferd;

cursor::cursor(object const &obj, boost::shared_ptr<mongo::DBClientCursor> ptr)
  : base_type(obj),
    cursor_(ptr)
{
}

cursor::~cursor() {}

int cursor::get_id() {
  return cursor_->getCursorId();
}

bool cursor::more() {
  return cursor_->more();
}

object cursor::next() {
  if (!cursor_->more())
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
  return object();
}

object cursor::min() {
  return object();
}

object cursor::max() {
  return object();
}

object cursor::snapshot() {
  return object();
}

object cursor::sort() {
  return object();
}

object cursor::where() {
  return object();
}


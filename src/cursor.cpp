
#include "cursor.hpp"
#include "mongodb.hpp"

using namespace flusspferd;

namespace mongodb_flusspferd {

cursor::cursor(object const &obj, boost::shared_ptr<mongo::DBClientCursor> ptr)
  : base_type(obj),
    cursor_(ptr)
{
}

cursor::~cursor() {}

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

} // namespace mongodb_flusspferd

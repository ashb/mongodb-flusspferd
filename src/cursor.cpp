
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

  return mongo_client::bson_to_object( cursor_->next() );
}

} // namespace mongodb_flusspferd

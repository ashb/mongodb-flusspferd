
#include "cursor.hpp"

using namespace flusspferd;

namespace mongodb_flusspferd {

cursor::cursor(object const &obj)
  : base_type(obj)
{
}

cursor::~cursor() {}

} // namespace mongodb_flusspferd

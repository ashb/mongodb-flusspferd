
#include "collection.hpp"

using namespace flusspferd;

namespace mongodb_flusspferd{

collection::collection(object const &obj, string db, string coll)
  : base_type(obj),
    db_(db),
    collection_name_(coll)
{
  define_property("db", db, read_only_property);
}


object collection::find(flusspferd::object query, flusspferd::object fields, int limit, int skip) {
}

object collection::find_one(flusspferd::object query, flusspferd::object fields) {
}

} // namespace

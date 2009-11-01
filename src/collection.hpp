
#ifndef GUARD_MONGODB_FLUSSPFERD_COLLECTION_HPP
#define GUARD_MONGODB_FLUSSPFERD_COLLECTION_HPP


#include <flusspferd.hpp>
#include <boost/shared_ptr.hpp>

namespace mongodb_flusspferd {

FLUSSPFERD_CLASS_DESCRIPTION(
    collection,
    (constructible, false)
    (full_name, "MongoDB.Collection")
    (constructor_name, "Collection")
    (methods,
      ("find", bind, find)
      ("findOne", bind, find_one)
    )
) {
public:
  collection(object const &obj, flusspferd::string db, flusspferd::string coll);

  flusspferd::object find(flusspferd::object query, flusspferd::object fields, int limit, int skip);
  flusspferd::object find_one(flusspferd::object query, flusspferd::object fields);
protected:
  flusspferd::string db_, collection_name_;

};

}

#endif

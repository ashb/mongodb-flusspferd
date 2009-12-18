
#ifndef GUARD_MONGODB_FLUSSPFERD_CURSOR_HPP
#define GUARD_MONGODB_FLUSSPFERD_CURSOR_HPP


#include <flusspferd.hpp>
#include <boost/shared_ptr.hpp>
#include <mongo/client/dbclient.h>

namespace mongodb_flusspferd {


FLUSSPFERD_CLASS_DESCRIPTION(
    cursor,
    (constructible, false)
    (full_name, "MongoDB.Cursor")
    (constructor_name, "Cursor")
    (methods,
      ("next", bind, next)
      ("more", bind, more)
      ("explain", bind, explain)
      ("hint", bind, hint)
      ("min", bind, min)
      ("max", bind, max)
      ("snapshot", bind, snapshot)
      ("sort", bind, sort)
      ("where", bind, where)
    )
    (properties,
      ("id", getter, get_id)
    )
) {
public:
  cursor(flusspferd::object const &obj, boost::shared_ptr<mongo::DBClientCursor> ptr);

  ~cursor();


  // Properties
  int get_id();

  // Methods
  flusspferd::object next();
  bool more();
  flusspferd::object explain();
  flusspferd::object hint();
  flusspferd::object min();
  flusspferd::object max();
  flusspferd::object snapshot();
  flusspferd::object sort();
  flusspferd::object where();
protected:

  boost::shared_ptr<mongo::DBClientCursor> cursor_;
};

}

#endif


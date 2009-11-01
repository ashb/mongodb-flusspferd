
#ifndef GUARD_MONGODB_FLUSSPFERD_CURSOR_HPP
#define GUARD_MONGODB_FLUSSPFERD_CURSOR_HPP


#include <flusspferd.hpp>
#include <boost/shared_ptr.hpp>

namespace mongodb_flusspferd {


FLUSSPFERD_CLASS_DESCRIPTION(
    cursor,
    (constructible, false)
    (full_name, "MongoDB.Cursor")
    (constructor_name, "Cursor")
    (methods,
      ("next", bind, next)
    )
) {
public:
  cursor(flusspferd::object const &obj);

  ~cursor();

  flusspferd::object next();
protected:
};

}

#endif


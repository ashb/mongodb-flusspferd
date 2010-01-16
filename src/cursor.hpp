
#ifndef GUARD_MONGODB_FLUSSPFERD_CURSOR_HPP
#define GUARD_MONGODB_FLUSSPFERD_CURSOR_HPP


#include <flusspferd.hpp>
#include <boost/shared_ptr.hpp>
#include <mongo/client/dbclient.h>

namespace mongodb_flusspferd {

class mongo_client;

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

  cursor(flusspferd::object const &obj, mongo_client *db_, 
    flusspferd::string ns_, mongo::BSONObj query,
    boost::optional<mongo::BSONObj> fields);

  ~cursor();


  // Properties
  int get_id();

  // Methods
  flusspferd::object next();
  bool more();
  flusspferd::object explain();
  flusspferd::object hint();
  flusspferd::object min(object key);
  flusspferd::object max(object key);
  flusspferd::object snapshot();
  void sort(flusspferd::call_context &x);
  flusspferd::object where();
protected:
  mongo_client &db_;
  flusspferd::string ns_;
  mongo::Query query_;
  boost::optional<mongo::BSONObj> fields_;
  int limit_, skip_;

  boost::shared_ptr<mongo::DBClientCursor> cursor_;

  // For the chain methods, assert cursor_ hasn't been generated yet
  void assert_no_cursor();
  boost::shared_ptr<mongo::DBClientCursor> mongo_cursor();

  void trace(flusspferd::tracer &trc);
};

}

#endif


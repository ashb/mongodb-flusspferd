
#ifndef GUARD_MONGODB_FLUSSPFERD_MONGODB_HPP
#define GUARD_MONGODB_FLUSSPFERD_MONGODB_HPP


#include <flusspferd.hpp>
#include <boost/shared_ptr.hpp>
#include <mongo/client/dbclient.h>

namespace mongodb_flusspferd {

FLUSSPFERD_CLASS_DESCRIPTION(
  mongo_client,
  (full_name, "MongoDB")
  (constructor_name, "MongoDB")
  (methods,
    ("close", bind, close)
    ("find", bind, find)
    ("insert", bind, insert)
    ("update", bind, update)
  )
) {
public:
  mongo_client(flusspferd::object const &obj, flusspferd::call_context &x);
  virtual ~mongo_client();

  static mongo::BSONObj object_to_bson(flusspferd::object obj);
  static mongo::BSONObj array_to_bson(flusspferd::array arr);

  void close();

  object find(flusspferd::string ns, flusspferd::object query, 
              flusspferd::object fields, int limit, int size);

  void insert(flusspferd::string ns, flusspferd::object obj);

  void update(flusspferd::string ns, flusspferd::object query, 
              flusspferd::object obj, boost::optional<bool> upsert); 
              // WTF is upsert? its in the mongo C++ api

  void remote(flusspferd::string ns, flusspferd::object query, 
              boost::optional<bool> just_one);
protected:
  mongo::DBClientConnection connection_;

};

}

#endif


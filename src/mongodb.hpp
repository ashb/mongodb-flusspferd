
#ifndef GUARD_MONGODB_FLUSSPFERD_MONGODB_HPP
#define GUARD_MONGODB_FLUSSPFERD_MONGODB_HPP


#include <flusspferd.hpp>
#include <boost/optional.hpp>
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
    ("findOne", bind, find_one)
    ("insert", bind, insert)
    ("update", bind, update)
    ("remove", bind, remove)
    ("getDatabaseNames", bind, get_dbs)
    ("getCollectionNames", bind, get_collections)
    ("exists", bind, exists)

    ("runCommand", bind, run_cmd)
  )
) {
public:
  mongo_client(flusspferd::object const &obj, flusspferd::call_context &x);
  virtual ~mongo_client();

  static mongo::BSONObj object_to_bson(flusspferd::object obj);
  static mongo::BSONObj array_to_bson(flusspferd::array arr);

  static flusspferd::object bson_to_object(mongo::BSONObj bson);
  static flusspferd::array bson_to_array(mongo::BSONElement e);

  void close();

  object find(flusspferd::value ns, flusspferd::object query,
              boost::optional<flusspferd::object> fields);

  object find_one(flusspferd::value ns, flusspferd::object query,
              boost::optional<flusspferd::object> fields);

  void insert(flusspferd::value ns, flusspferd::object obj);

  void update(flusspferd::value ns, flusspferd::object query,
              flusspferd::object obj, boost::optional<bool> upsert,
              boost::optional<bool> multi);

  void remove(flusspferd::value ns, flusspferd::object query,
              boost::optional<bool> just_one);

  object get_dbs();
  object get_collections(flusspferd::value ns);
  bool exists(flusspferd::value ns);

  object run_cmd(flusspferd::value db, object cmd);
protected:
  friend class cursor;
  mongo::DBClientConnection connection_;

};

}

#endif


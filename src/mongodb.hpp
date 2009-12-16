
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

  object find(std::string const &ns, flusspferd::object query,
              boost::optional<int> limit, boost::optional<int> skip,
              boost::optional<flusspferd::object> fields);

  object find_one(std::string const &ns, flusspferd::object query,
              boost::optional<flusspferd::object> fields);

  void insert(std::string const &ns, flusspferd::object obj);

  void update(std::string const &ns, flusspferd::object query,
              flusspferd::object obj, boost::optional<bool> upsert,
              boost::optional<bool> multi);

  void remove(std::string const &ns, flusspferd::object query,
              boost::optional<bool> just_one);

  object get_dbs();
  object get_collections(std::string const &ns);
  bool exists(std::string const &ns);

  object run_cmd(std::string const &db, object cmd);
protected:
  mongo::DBClientConnection connection_;

};

}

#endif


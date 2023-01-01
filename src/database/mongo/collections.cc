#include <database/mongo.h>

int Mongo::create_collections() {
  WRAP_FUNC(this->create_x_collection("users", "id:int64;name;login;node_id;type;created_at;updated_at;"
                                               "public_gists:int64;public_repos:int64;following:int64;followers:int64"))
  WRAP_FUNC(this->create_x_collection("orgs", "id:int64;login;node_id"))
  WRAP_FUNC(this->create_x_collection("emojis", "name;url"))
  WRAP_FUNC(this->create_x_collection("gitignores", "name;source"))
  WRAP_FUNC(this->create_x_collection("licenses", "key;name"))
  return EXIT_SUCCESS;
}

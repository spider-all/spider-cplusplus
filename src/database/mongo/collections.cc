#include <database/mongo.h>

int Mongo::create_collections() {
  std::string users_schema = "id$int64:name:login:node_id:type:created_at:updated_at:"
                             "public_gists$int64:public_repos$int64:following$int64:followers$int64";
  WRAP_FUNC(this->create_x_collection("users", users_schema))
  std::string orgs_schema = "id$int64:login:node_id";
  WRAP_FUNC(this->create_x_collection("orgs", orgs_schema))
  std::string emojis_schema = "name:url";
  WRAP_FUNC(this->create_x_collection("emojis", emojis_schema))
  std::string gitignores_schema = "name:source";
  WRAP_FUNC(this->create_x_collection("gitignores", gitignores_schema))
  std::string licenses_schema = "key:name";
  WRAP_FUNC(this->create_x_collection("licenses", licenses_schema))
  return EXIT_SUCCESS;
}

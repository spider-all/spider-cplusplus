#include <database/duckdb.h>

int DuckDBDatabase::create_collections() {
  // Create version tracking tables
  this->execute(
      "CREATE TABLE IF NOT EXISTS versions ("
      "  type TEXT PRIMARY KEY,"
      "  version BIGINT"
      ")");

  this->execute(
      "CREATE TABLE IF NOT EXISTS version_tracking ("
      "  type TEXT,"
      "  key TEXT,"
      "  version BIGINT,"
      "  PRIMARY KEY (type, key)"
      ")");

  // Create data tables
  WRAP_FUNC(this->create_x_collection("users", "id:int64;name;login;node_id;type;created_at;updated_at;"
                                               "public_gists:int64;public_repos:int64;following:int64;followers:int64"))
  WRAP_FUNC(this->create_x_collection("orgs", "id:int64;login;node_id"))
  WRAP_FUNC(this->create_x_collection("emojis", "name;url"))
  WRAP_FUNC(this->create_x_collection("gitignores", "name;source"))
  WRAP_FUNC(this->create_x_collection("licenses", "key;name"))
  WRAP_FUNC(this->create_x_collection("repos", "id:int64;name;owner;full_name"))
  WRAP_FUNC(this->create_x_collection("branches", "repo;name;commit"))
  WRAP_FUNC(this->create_x_collection("commits", "sha;owner;repo;branch"))
  return EXIT_SUCCESS;
}

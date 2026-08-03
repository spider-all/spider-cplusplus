#include <database/sqlite.h>

int SQLiteDatabase::create_collections() {
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
  WRAP_FUNC(this->create_x_collection("users", "id:int64;login;node_id;type;name;company;blog;location;"
                                               "email;hireable;bio;created_at;updated_at;"
                                               "public_gists:int64;public_repos:int64;following:int64;followers:int64"))
  WRAP_FUNC(this->create_x_collection("orgs", "id:int64;login;node_id;description"))
  WRAP_FUNC(this->create_x_collection("repos", "id:int64;node_id;name;full_name;xprivate;owner;owner_type;"
                                                "description;fork;created_at;updated_at;pushed_at;homepage;"
                                                "size:int64;stargazers_count:int64;watchers_count:int64;"
                                                "forks_count:int64;language;license;forks:int64;"
                                                "open_issues:int64;watchers:int64;default_branch"))
  return EXIT_SUCCESS;
}

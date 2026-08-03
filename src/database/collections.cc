#include <database/sqlite.h>

int SQLiteDatabase::create_collections() {
  // Create data tables
  WRAP_FUNC(this->create_x_collection("users", "id:int64;login;node_id;type;name;company;blog;location;"
                                               "email;hireable;bio;created_at;updated_at;"
                                               "public_gists:int64;public_repos:int64;following:int64;followers:int64;"
                                               "data_created_at:int64;data_updated_at:int64;data_version:int64"))
  WRAP_FUNC(this->create_x_collection("orgs", "id:int64;login;node_id;description;followers:int64;"
                                              "data_created_at:int64;data_updated_at:int64;data_version:int64"))
  WRAP_FUNC(this->create_x_collection("repos", "id:int64;node_id;name;full_name;xprivate;owner;owner_type;"
                                                "description;fork;created_at;updated_at;pushed_at;homepage;"
                                                "size:int64;stargazers_count:int64;watchers_count:int64;"
                                                "forks_count:int64;language;license;forks:int64;"
                                                "open_issues:int64;watchers:int64;default_branch;"
                                                "data_created_at:int64;data_updated_at:int64;data_version:int64"))

  WRAP_FUNC(this->execute(
      "CREATE TABLE IF NOT EXISTS user_following ("
      "  upstream_user_id BIGINT,"
      "  downstream_user_id BIGINT,"
      "  data_created_at BIGINT,"
      "  data_updated_at BIGINT,"
      "  data_version BIGINT,"
      "  PRIMARY KEY (upstream_user_id, downstream_user_id)"
      ")"))
  WRAP_FUNC(this->execute(
      "CREATE TABLE IF NOT EXISTS repo_star ("
      "  user_id BIGINT,"
      "  repo_id BIGINT,"
      "  data_created_at BIGINT,"
      "  data_updated_at BIGINT,"
      "  data_version BIGINT,"
      "  PRIMARY KEY (user_id, repo_id)"
      ")"))

  return EXIT_SUCCESS;
}

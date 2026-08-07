#include <database/sqlite.h>

int SQLiteDatabase::create_collections() {
  // Create data tables
  WRAP_FUNC(this->create_x_collection(
      "users",
      "id:int64;login;node_id;type;name;company;blog;location;"
      "email;hireable;bio;created_at;updated_at;"
      "public_gists:int64;public_repos:int64;following:int64;followers:int64;"
      "data_created_at:int64;data_updated_at:int64;data_version:int64"))
  WRAP_FUNC(this->create_x_collection(
      "orgs",
      "id:int64;login;node_id;description;followers:int64;"
      "data_created_at:int64;data_updated_at:int64;data_version:int64"))
  WRAP_FUNC(this->create_x_collection(
      "repos",
      "id:int64;node_id;name;full_name;xprivate;owner;owner_type;"
      "description;fork;created_at;updated_at;pushed_at;homepage;"
      "size:int64;stargazers_count:int64;watchers_count:int64;"
      "forks_count:int64;language;license;forks:int64;"
      "open_issues:int64;watchers:int64;default_branch;"
      "data_created_at:int64;data_updated_at:int64;data_version:int64"))

  WRAP_FUNC(this->execute(
      "CREATE INDEX IF NOT EXISTS users_data_version_index ON users (data_version)"))
  WRAP_FUNC(this->execute(
      "CREATE INDEX IF NOT EXISTS orgs_data_version_index ON orgs (data_version)"))
  WRAP_FUNC(this->execute(
      "CREATE INDEX IF NOT EXISTS repos_data_version_index ON repos (data_version)"))
  WRAP_FUNC(this->execute(
      "CREATE INDEX IF NOT EXISTS repos_stargazers_count_index ON repos (stargazers_count)"))

  return EXIT_SUCCESS;
}

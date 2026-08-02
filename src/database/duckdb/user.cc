#include <database/duckdb.h>

int DuckDBDatabase::upsert_user(User user) {
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO users (id, login, node_id, type, name, company, blog, location, "
      "email, hireable, bio, created_at, updated_at, public_gists, public_repos, following, followers) "
      "VALUES ({}, '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', {}, '{}', '{}', '{}', {}, {}, {}, {})",
      user.id,
      this->escape(user.login),
      this->escape(user.node_id),
      this->escape(user.type),
      this->escape(user.name),
      this->escape(user.company),
      this->escape(user.blog),
      this->escape(user.location),
      this->escape(user.email),
      user.hireable ? "true" : "false",
      this->escape(user.bio),
      this->escape(user.created_at),
      this->escape(user.updated_at),
      user.public_gists,
      user.public_repos,
      user.following,
      user.followers);
  return this->execute(sql);
}

int DuckDBDatabase::upsert_user_with_version(User user, enum request_type type) {
  WRAP_FUNC(this->upsert_user(user))
  WRAP_FUNC(this->update_version(std::to_string(user.id), type))
  return EXIT_SUCCESS;
}

std::vector<User> DuckDBDatabase::list_usersx(common_args args) {
  std::vector<User> users;
  try {
    auto result = this->con->Query("SELECT id, login, node_id, type, name, company, blog, location, "
                                   "email, hireable, bio, created_at, updated_at, public_gists, "
                                   "public_repos, following, followers FROM users");
    if (result->HasError()) {
      spdlog::error("DuckDB error: {}", result->GetError());
      return users;
    }
    for (duckdb::idx_t row = 0; row < result->RowCount(); row++) {
      std::cout << result->GetValue(1, row).ToString() << "\n";
    }
  } catch (const std::exception &e) {
    spdlog::error("DuckDB error: {}", e.what());
  }
  return users;
}

std::vector<std::string> DuckDBDatabase::list_users_random(enum request_type type) {
  return this->list_x_random("users", "login", type);
}

int64_t DuckDBDatabase::count_user() {
  return this->count_x("users");
}
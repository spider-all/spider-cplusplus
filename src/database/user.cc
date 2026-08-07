#include <database/sqlite.h>

int SQLiteDatabase::upsert_user(User user) {
  int64_t now = this->current_timestamp();
  int64_t version = this->initial_data_version("users");
  std::string sql = fmt::format(
      "INSERT INTO users (id, login, node_id, type, name, company, blog, location, "
      "email, hireable, bio, created_at, updated_at, public_gists, public_repos, following, followers, "
      "data_created_at, data_updated_at, data_version) "
      "VALUES ({}, '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', {}, '{}', '{}', '{}', {}, {}, {}, {}, "
      "{}, {}, {}) "
      "ON CONFLICT(id) DO UPDATE SET "
      "login = excluded.login, "
      "node_id = excluded.node_id, "
      "type = excluded.type, "
      "name = excluded.name, "
      "company = excluded.company, "
      "blog = excluded.blog, "
      "location = excluded.location, "
      "email = excluded.email, "
      "hireable = excluded.hireable, "
      "bio = excluded.bio, "
      "created_at = excluded.created_at, "
      "updated_at = excluded.updated_at, "
      "public_gists = excluded.public_gists, "
      "public_repos = excluded.public_repos, "
      "following = excluded.following, "
      "followers = excluded.followers, "
      "data_updated_at = {}",
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
      user.followers,
      now,
      now,
      version,
      now);
  return this->execute(sql);
}

int SQLiteDatabase::upsert_user_with_version(User user, enum request_type type) {
  WRAP_FUNC(this->upsert_user(user))
  return EXIT_SUCCESS;
}

std::vector<User> SQLiteDatabase::list_usersx(common_args args) {
  std::vector<User> users;
  std::string sql = "SELECT id, login, node_id, type, name, company, blog, location, "
                    "email, hireable, bio, created_at, updated_at, public_gists, "
                    "public_repos, following, followers FROM users";
  auto start = std::chrono::steady_clock::now();
  try {
    SQLite::Statement query(*this->db, sql);
    while (query.executeStep()) {
      std::cout << query.getColumn(1).getString() << "\n";
    }
    this->log_query(sql, start);
  } catch (const std::exception &e) {
    auto cost = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    spdlog::error("SQLite error: {}, cost={}, sql={}", e.what(), SQLiteDatabase::format_sql_cost(cost), sql);
  }
  return users;
}

std::vector<std::string> SQLiteDatabase::list_users_random() {
  return this->list_x_random("users", "id;login");
}

int64_t SQLiteDatabase::count_user() {
  return this->count_x("users");
}

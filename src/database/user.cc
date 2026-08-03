#include <database/sqlite.h>

int SQLiteDatabase::upsert_user(User user) {
  std::string sql = fmt::format(
      "INSERT INTO users (id, login, node_id, type, name, company, blog, location, "
      "email, hireable, bio, created_at, updated_at, public_gists, public_repos, following, followers, "
      "data_created_at, data_updated_at) "
      "VALUES ({}, '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', {}, '{}', '{}', '{}', {}, {}, {}, {}, "
      "CAST(strftime('%s','now') AS INTEGER), CAST(strftime('%s','now') AS INTEGER)) "
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
      "data_updated_at = CAST(strftime('%s','now') AS INTEGER)",
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

int SQLiteDatabase::upsert_user_with_version(User user, enum request_type type) {
  WRAP_FUNC(this->upsert_user(user))
  WRAP_FUNC(this->update_version(std::to_string(user.id), type))
  return EXIT_SUCCESS;
}

std::vector<User> SQLiteDatabase::list_usersx(common_args args) {
  std::vector<User> users;
  try {
    SQLite::Statement query(*this->db, "SELECT id, login, node_id, type, name, company, blog, location, "
                                       "email, hireable, bio, created_at, updated_at, public_gists, "
                                       "public_repos, following, followers FROM users");
    while (query.executeStep()) {
      std::cout << query.getColumn(1).getString() << "\n";
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
  }
  return users;
}

std::vector<std::string> SQLiteDatabase::list_users_random(enum request_type type) {
  return this->list_x_random("users", "login", type);
}

int64_t SQLiteDatabase::count_user() {
  return this->count_x("users");
}

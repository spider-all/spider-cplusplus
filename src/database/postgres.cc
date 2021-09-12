#include <database/postgres.h>

Postgres::Postgres(const std::string &host) {
  db.pg = new pqxx::connection(host);
}

Postgres::~Postgres() { db.pg->close(); }

int Postgres::initialize() {
  const std::string CreateSentence[] = {
      "CREATE TABLE IF NOT EXISTS users ("
      "id INT8 NOT NULL PRIMARY KEY, "
      "login TEXT NOT NULL, "
      "node_id TEXT NOT NULL, "
      "type TEXT NOT NULL, "
      "name TEXT NOT NULL, "
      "company TEXT, "
      "blog TEXT, "
      "location TEXT, "
      "email TEXT, "
      "hireable BOOL, "
      "bio TEXT, "
      "created_at TEXT NOT NULL, "
      "updated_at TEXT NOT NULL, "
      "public_gists INT8 NOT NULL, "
      "public_repos INT8 NOT NULL, "
      "following INT8 NOT NULL, "
      "followers INT8 NOT NULL);",
      "CREATE TABLE IF NOT EXISTS orgs ("
      "id INT8 NOT NULL PRIMARY KEY, "
      "login TEXT NOT NULL, "
      "node_id TEXT NOT NULL, "
      "description TEXT NOT NULL);",
  };
  pqxx::work txn = pqxx::work(*db.pg);
  for (const std::string &sql : CreateSentence) {
    spdlog::info("Initialize sql: {}", sql);
    try {
      txn.exec(sql);
    } catch (std::exception const &e) {
      spdlog::error("Execute SQL with error: {}", e.what());
    }
  }

  try {
    txn.commit();
  } catch (std::exception const &e) {
    spdlog::error("Execute SQL with error: {}", e.what());
  }

  return EXIT_SUCCESS;
}

int Postgres::create_user(User user) { return EXIT_SUCCESS; }

std::vector<User> Postgres::list_usersx(common_args args) {
    std::vector<User> users;
    return users;
}

int Postgres::create_org(Org org) { return EXIT_SUCCESS; }

std::vector<std::string> Postgres::list_users() {
  std::vector<std::string> users;
  return users;
}

std::vector<std::string> Postgres::list_orgs() {
  std::vector<std::string> orgs;
  return orgs;
}

int64_t Postgres::count_user() {
  return 0;
}

int64_t Postgres::count_org() {
  return 0;
}

int Postgres::create_emoji(std::vector<Emoji> emojis) {
  return EXIT_SUCCESS;
}

int64_t Postgres::count_emoji() {
  return 0;
}

int64_t Postgres::count_gitignore() {
  return 0;
}

int Postgres::create_gitignore(Gitignore gitignore) {
  return EXIT_SUCCESS;
}

int64_t Postgres::count_license() {
  return 0;
}

int Postgres::create_license(License license) {
  return EXIT_SUCCESS;
}

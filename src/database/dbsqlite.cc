#include <database/dbsqlite.h>

DBSQLite::DBSQLite(const std::string &path) {
  try {
    unsigned flags = unsigned(SQLite::OPEN_CREATE) | unsigned(SQLite::OPEN_READWRITE) | unsigned(SQLite::OPEN_FULLMUTEX);
    db.sqlite = new SQLite::Database(path, int(flags));
  } catch (std::exception &e) {
    spdlog::error("Open database with error: {}", e.what());
    code = DATABASE_OPEN_ERROR;
  }
}

DBSQLite::~DBSQLite() {
  delete db.sqlite;
}

int DBSQLite::initialize() {
  const std::string CreateSentence[] = {
      "CREATE TABLE IF NOT EXISTS `users` ("
      "`id` NUMERIC NOT NULL PRIMARY KEY, "
      "`login` TEXT NOT NULL, "
      "`node_id` TEXT NOT NULL, "
      "`type` TEXT NOT NULL, "
      "`name` TEXT NOT NULL, "
      "`company` TEXT, "
      "`blog` TEXT, "
      "`location` TEXT, "
      "`email` TEXT, "
      "`hireable` NUMERIC, "
      "`bio` TEXT, "
      "`created_at` TEXT NOT NULL, "
      "`updated_at` TEXT NOT NULL, "
      "`public_gists` NUMERIC NOT NULL, "
      "`public_repos` NUMERIC NOT NULL, "
      "`following` NUMERIC NOT NULL, "
      "`followers` NUMERIC NOT NULL);",
  };
  for (const std::string &sql : CreateSentence) {
    spdlog::info("Initialize sql: {}", sql);
    try {
      db.sqlite->exec(sql);
    } catch (const std::exception &e) {
      spdlog::error("Execute SQL with error: {}", e.what());
      return DATABASE_SQL_ERROR;
    }
  }
  return EXIT_SUCCESS;
}

int DBSQLite::create_user(user user) {
  auto *query = new SQLite::Statement(*db.sqlite, "SELECT `id` FROM `users` WHERE `id` = ?");
  query->bind(1, user.id);
  std::string create_sql;
  if (query->executeStep()) {
    create_sql = "UPDATE `users` SET `id` = ?, `login` = ?, `node_id` = ?, `type` = ?, `name` = ?, `company` = ?, `blog` = ?, `location` = ?, `email` = ?, `hireable` = ?, `bio` = ?, `created_at` = ?, `updated_at` = ?, `public_gists` = ?, `public_repos` = ?, `following` = ?, `followers` = ? WHERE `id` = ?";
  } else {
    create_sql = "INSERT INTO `users` VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
  }

  auto *create_st = new SQLite::Statement(*db.sqlite, create_sql);

  create_st->bind(1, user.id);
  create_st->bind(2, user.login);
  create_st->bind(3, user.node_id);
  create_st->bind(4, user.type);
  create_st->bind(5, user.name);
  create_st->bind(6, user.company);
  create_st->bind(7, user.blog);
  create_st->bind(8, user.location);
  create_st->bind(9, user.email);
  create_st->bind(10, user.hireable);
  create_st->bind(11, user.bio);
  create_st->bind(12, user.created_at);
  create_st->bind(13, user.updated_at);
  create_st->bind(14, user.public_gists);
  create_st->bind(15, user.public_repos);
  create_st->bind(16, user.following);
  create_st->bind(17, user.followers);

  if (query->hasRow()) {
    create_st->bind(18, user.id);
  }

  delete query;

  try {
    create_st->exec();
  } catch (const std::exception &e) {
    spdlog::error("Insert user with error: {}", e.what());
    return DATABASE_SQL_ERROR;
  }

  delete create_st;

  return EXIT_SUCCESS;
}

std::vector<std::string> DBSQLite::list_users() {
  std::vector<std::string> users;
  auto *query = new SQLite::Statement(*db.sqlite, "SELECT `login` FROM `users` ORDER BY random() limit 100");
  try {
    while (query->executeStep()) {
      std::string name = query->getColumn(0);
      users.push_back(name);
    }
  } catch (const std::exception &e) {
    spdlog::error("Query user with error: {}", e.what());
  }
  delete query;
  return users;
}

int DBSQLite::count_user() {
  int count = 0;
  auto *query = new SQLite::Statement(*db.sqlite, "SELECT COUNT(*) FROM `users`");
  try {
    while (query->executeStep()) {
      count = query->getColumn(0);
    }
  } catch (const std::exception &e) {
    spdlog::error("Query user with error: {}", e.what());
  }
  delete query;
  return count;
}

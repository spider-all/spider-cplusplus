#include <database/sqlite3.h>

SQLite3::SQLite3(const std::string &path) {
  this->path = path;
}

SQLite3::~SQLite3() { delete this->db; }

int SQLite3::initialize() {
  try {
    unsigned flags = unsigned(SQLite::OPEN_CREATE) |
                     unsigned(SQLite::OPEN_READWRITE) |
                     unsigned(SQLite::OPEN_FULLMUTEX);
    this->db = new SQLite::Database(path, int(flags));
  } catch (std::exception &e) {
    spdlog::error("Open database with error: {}", e.what());
    code = DATABASE_OPEN_ERROR;
  }
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
      "CREATE TABLE IF NOT EXISTS `orgs` ("
      "`id` NUMERIC NOT NULL PRIMARY KEY, "
      "`login` TEXT NOT NULL, "
      "`node_id` TEXT NOT NULL, "
      "`description` TEXT NOT NULL);",
      "CREATE TABLE IF NOT EXISTS `emojis` ("
      "`name` TEXT NOT NULL PRIMARY KEY, "
      "`url` TEXT NOT NULL);",
      "CREATE TABLE IF NOT EXISTS `gitignores` ("
      "`name` TEXT NOT NULL PRIMARY KEY, "
      "`source` TEXT NOT NULL);",
      "CREATE TABLE IF NOT EXISTS `licenses` ("
      "`key` TEXT NOT NULL PRIMARY KEY, "
      "`name` TEXT NOT NULL, "
      "`spdx_id` TEXT NOT NULL, "
      "`node_id` TEXT NOT NULL, "
      "`description` TEXT, "
      "`implementation` TEXT, "
      "`permissions` TEXT, "
      "`conditions` TEXT, "
      "`limitations` TEXT, "
      "`body` TEXT NOT NULL, "
      "`featured` NUMERIC);",
  };
  for (const std::string &sql : CreateSentence) {
    spdlog::info("Initialize sql: {}", sql);
    try {
      this->db->exec(sql);
    } catch (const std::exception &e) {
      spdlog::error("Execute SQL with error: {}", e.what());
      return DATABASE_SQL_ERROR;
    }
  }
  return EXIT_SUCCESS;
}

int SQLite3::initialize_version() {
  return 0;
}

int SQLite3::update_version(std::string key, enum request_type type) {
  return 0;
}
int SQLite3::update_version(std::vector<std::string> key, enum request_type type) {
  return 0;
}
int SQLite3::incr_version(enum request_type type) {
  return 0;
}

int SQLite3::upsert_user(User user) {
  return 0;
}
int SQLite3::upsert_user_with_version(User user, enum request_type type) {
  return 0;
}
int64_t SQLite3::count_user() {
  return 0;
}
std::vector<std::string> SQLite3::list_users_random(enum request_type type) {
  return std::vector<std::string>();
}

std::vector<User> SQLite3::list_usersx(common_args args) {
  return std::vector<User>();
}

int SQLite3::upsert_org(Org org) {
  return 0;
}
int SQLite3::upsert_org(std::vector<Org> orgs) {
  return 0;
}
int SQLite3::upsert_org_with_version(Org org, enum request_type type) {
  return 0;
}
int SQLite3::upsert_org_with_version(std::vector<Org> orgs, enum request_type type) {
  return 0;
}
int64_t SQLite3::count_org() {
  return 0;
}
std::vector<std::string> SQLite3::list_orgs_random(enum request_type type) {
  return std::vector<std::string>();
}

int SQLite3::upsert_emoji(std::vector<Emoji> emojis) {
  return 0;
}
int64_t SQLite3::count_emoji() {
  return 0;
}

int SQLite3::upsert_gitignore(Gitignore gitignore) {
  return 0;
}
int64_t SQLite3::count_gitignore() {
  return 0;
}

int SQLite3::upsert_license(License license) {
  return 0;
}
int SQLite3::upsert_license_with_version(License license, enum request_type type) {
  return 0;
}
int64_t SQLite3::count_license() {
  return 0;
}

int SQLite3::upsert_repo(Repo repo) {
  return 0;
}
int SQLite3::upsert_repo(std::vector<Repo> repos) {
  return 0;
}
int SQLite3::upsert_repo_with_version(Repo repo, enum request_type type) {
  return 0;
}
int SQLite3::upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) {
  return 0;
}
std::vector<std::string> SQLite3::list_repos_random(enum request_type type) {
  return std::vector<std::string>();
}
int64_t SQLite3::count_repo() {
  return 0;
}

int SQLite3::upsert_branch(Branch branch) {
  return 0;
}
int SQLite3::upsert_branch(std::vector<Branch> branches) {
  return 0;
}
int SQLite3::upsert_branch_with_version(Branch branch, enum request_type type) {
  return 0;
}
int SQLite3::upsert_branch_with_version(std::vector<Branch> branches, enum request_type type) {
  return 0;
}
std::vector<std::string> SQLite3::list_branches_random(enum request_type type) {
  return std::vector<std::string>();
}

int SQLite3::upsert_commit(Commit commit) {
  return 0;
}
int SQLite3::upsert_commit(std::vector<Commit> commits) {
  return 0;
}
int SQLite3::upsert_commit_with_version(Commit commit, enum request_type type) {
  return 0;
}
int SQLite3::upsert_commit_with_version(std::vector<Commit> commits, enum request_type type) {
  return 0;
}

// int64_t SQLite3::count_x(const std::string &table, const std::string &field) {
//   int64_t count = 0;
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT COUNT(" + field + ") FROM `" + table + "`");
//   try {
//     while (query->executeStep()) {
//       count = query->getColumn(0);
//     }
//   } catch (const std::exception &e) {
//     spdlog::error("Query {} with error: {}", table, e.what());
//     return SQL_EXEC_ERROR;
//   }
//   delete query;
//   return count;
// }

// int SQLite3::create_user(User user) {
//   auto *query = new SQLite::Statement(
//       *db.sqlite, "SELECT `id` FROM `users` WHERE `id` = ?");
//   query->bind(1, user.id);
//   std::string create_sql;
//   if (query->executeStep()) {
//     create_sql =
//         "UPDATE `users` SET `id` = ?, `login` = ?, `node_id` = ?, `type` = ?, "
//         "`name` = ?, `company` = ?, `blog` = ?, `location` = ?, `email` = ?, "
//         "`hireable` = ?, `bio` = ?, `created_at` = ?, `updated_at` = ?, "
//         "`public_gists` = ?, `public_repos` = ?, `following` = ?, `followers` "
//         "= ? WHERE `id` = ?";
//   } else {
//     create_sql = "INSERT INTO `users` VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
//   }

//   auto *create_st = new SQLite::Statement(*db.sqlite, create_sql);

//   create_st->bind(1, user.id);
//   create_st->bind(2, user.login);
//   create_st->bind(3, user.node_id);
//   create_st->bind(4, user.type);
//   create_st->bind(5, user.name);
//   create_st->bind(6, user.company);
//   create_st->bind(7, user.blog);
//   create_st->bind(8, user.location);
//   create_st->bind(9, user.email);
//   create_st->bind(10, user.hireable);
//   create_st->bind(11, user.bio);
//   create_st->bind(12, user.created_at);
//   create_st->bind(13, user.updated_at);
//   create_st->bind(14, user.public_gists);
//   create_st->bind(15, user.public_repos);
//   create_st->bind(16, user.following);
//   create_st->bind(17, user.followers);

//   if (query->hasRow()) {
//     create_st->bind(18, user.id);
//   }

//   delete query;

//   try {
//     create_st->exec();
//   } catch (const std::exception &e) {
//     spdlog::error("Insert user with error: {}", e.what());
//     return DATABASE_SQL_ERROR;
//   }

//   delete create_st;

//   return EXIT_SUCCESS;
// }

// int SQLite3::create_emoji(std::vector<Emoji> emojis) {
//   for (const Emoji &emoji : emojis) {
//     auto *query = new SQLite::Statement(*db.sqlite, "SELECT `name` FROM `emojis` WHERE `name` = ?");
//     query->bind(1, emoji.name);
//     std::string create_sql;
//     if (query->executeStep()) {
//       create_sql = "UPDATE `emojis` SET `name` = ?, `url` = ? WHERE `name` = ?";
//     } else {
//       create_sql = "INSERT INTO `emojis` VALUES (?, ?)";
//     }

//     auto *create_st = new SQLite::Statement(*db.sqlite, create_sql);

//     create_st->bind(1, emoji.name);
//     create_st->bind(2, emoji.url);

//     if (query->hasRow()) {
//       create_st->bind(3, emoji.name);
//     }

//     delete query;

//     try {
//       create_st->exec();
//     } catch (const std::exception &e) {
//       spdlog::error("Insert emoji with error: {}", e.what());
//       return DATABASE_SQL_ERROR;
//     }

//     delete create_st;
//   }
//   return EXIT_SUCCESS;
// }

// int64_t SQLite3::count_emoji() {
//   return count_x("emojis", "name");
// }

// int SQLite3::create_gitignore(Gitignore gitignore) {
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT `name` FROM `gitignores` WHERE `name` = ?");
//   query->bind(1, gitignore.name);
//   std::string create_sql;
//   if (query->executeStep()) {
//     create_sql = "UPDATE `gitignores` SET `name` = ?, `source` = ? WHERE `name` = ?";
//   } else {
//     create_sql = "INSERT INTO `gitignores` VALUES (?, ?)";
//   }

//   auto *create_st = new SQLite::Statement(*db.sqlite, create_sql);

//   create_st->bind(1, gitignore.name);
//   create_st->bind(2, gitignore.source);

//   if (query->hasRow()) {
//     create_st->bind(3, gitignore.name);
//   }

//   delete query;

//   try {
//     create_st->exec();
//   } catch (const std::exception &e) {
//     spdlog::error("Insert gitignore with error: {}", e.what());
//     return DATABASE_SQL_ERROR;
//   }

//   delete create_st;
//   return EXIT_SUCCESS;
// }

// int64_t SQLite3::count_gitignore() {
//   return count_x("gitignores", "name");
// }

// int64_t SQLite3::count_license() {
//   return count_x("licenses", "key");
// }

// int SQLite3::create_license(License license) {
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT `key` FROM `licenses` WHERE `key` = ?");
//   query->bind(1, license.key);
//   std::string create_sql;
//   if (query->executeStep()) {
//     create_sql = "UPDATE `licenses` SET `key` = ?, `name` = ?, `spdx_id` = ?, `node_id` = ?, `description` = ?, `implementation` = ?, `permissions` = ?, `conditions` = ?, `limitations` = ?, `body` = ?, `featured` = ? WHERE `name` = ?";
//   } else {
//     create_sql = "INSERT INTO `licenses` VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
//   }

//   auto *create_st = new SQLite::Statement(*db.sqlite, create_sql);

//   create_st->bind(1, license.key);
//   create_st->bind(2, license.name);
//   create_st->bind(3, license.spdx_id);
//   create_st->bind(4, license.node_id);
//   create_st->bind(5, license.description);
//   create_st->bind(6, license.implementation);
//   create_st->bind(7, license.permissions);
//   create_st->bind(8, license.conditions);
//   create_st->bind(9, license.limitations);
//   create_st->bind(10, license.body);
//   create_st->bind(11, license.featured);

//   if (query->hasRow()) {
//     create_st->bind(12, license.key);
//   }

//   delete query;

//   try {
//     create_st->exec();
//   } catch (const std::exception &e) {
//     spdlog::error("Insert gitignore with error: {}", e.what());
//     return DATABASE_SQL_ERROR;
//   }

//   delete create_st;
//   return EXIT_SUCCESS;
// }

// int SQLite3::create_org(Org org) {
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT `id` FROM `orgs` WHERE `id` = ?");
//   query->bind(1, org.id);
//   std::string create_sql;
//   if (query->executeStep()) {
//     create_sql = "UPDATE `orgs` SET `id` = ?, `login` = ?, `node_id` = ?, `description` = ? WHERE `id` = ?";
//   } else {
//     create_sql = "INSERT INTO `orgs` VALUES (?, ?, ?, ?)";
//   }

//   auto *create_st = new SQLite::Statement(*db.sqlite, create_sql);

//   create_st->bind(1, org.id);
//   create_st->bind(2, org.login);
//   create_st->bind(3, org.node_id);
//   create_st->bind(4, org.description);

//   if (query->hasRow()) {
//     create_st->bind(5, org.id);
//   }

//   delete query;

//   try {
//     create_st->exec();
//   } catch (const std::exception &e) {
//     spdlog::error("Insert org with error: {}", e.what());
//     return DATABASE_SQL_ERROR;
//   }

//   delete create_st;

//   return EXIT_SUCCESS;
// }

// std::vector<User> SQLite3::list_usersx(common_args args) {
//   std::vector<User> users;
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT `login` FROM `users` limit " + std::to_string(args.limit) + " offset " + std::to_string((args.page - 1) * args.limit));
//   try {
//     while (query->executeStep()) {
//       User user;
//       std::string name = query->getColumn(0);
//       user.login = name;
//       users.push_back(user);
//     }
//   } catch (const std::exception &e) {
//     spdlog::error("Query user with error: {}", e.what());
//   }
//   delete query;
//   return users;
// }

// std::vector<std::string> SQLite3::list_users() {
//   std::vector<std::string> users;
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT `login` FROM `users` ORDER BY random() limit 100");
//   try {
//     while (query->executeStep()) {
//       std::string name = query->getColumn(0);
//       users.push_back(name);
//     }
//   } catch (const std::exception &e) {
//     spdlog::error("Query user with error: {}", e.what());
//   }
//   delete query;
//   return users;
// }

// std::vector<std::string> SQLite3::list_orgs() {
//   std::vector<std::string> orgs;
//   auto *query = new SQLite::Statement(*db.sqlite, "SELECT `login` FROM `orgs` ORDER BY random() limit 100");
//   try {
//     while (query->executeStep()) {
//       std::string name = query->getColumn(0);
//       orgs.push_back(name);
//     }
//   } catch (const std::exception &e) {
//     spdlog::error("Query user with error: {}", e.what());
//   }
//   delete query;
//   return orgs;
// }

// int64_t SQLite3::count_user() {
//   return count_x("users", "id");
// }

// int64_t SQLite3::count_org() {
//   return count_x("orgs", "id");
// }

#include <database/mysql.h>

MySQL::MySQL(const std::string &host, const std::string &user,
             const std::string &password, const std::string &db_name, unsigned int port) {
  mysql_library_init(0, nullptr, nullptr);
  db.mysql = mysql_init(nullptr);
  if (mysql_real_connect(db.mysql, host.c_str(), user.c_str(), password.c_str(),
                         db_name.c_str(), port, nullptr, 0) == nullptr) {
    spdlog::error("Database connect with error {}", mysql_error(db.mysql));
  }
}

MySQL::~MySQL() {
  if (db.mysql != nullptr) {
    mysql_close(db.mysql);
  }
  mysql_library_end();
}

int MySQL::initialize() {
  return EXIT_SUCCESS;
}

int MySQL::create_user(User user) { return EXIT_SUCCESS; }

int MySQL::create_org(Org org) { return EXIT_SUCCESS; }

std::vector<std::string> MySQL::list_users() {
  std::vector<std::string> users;
  return users;
}

std::vector<User> MySQL::list_usersx(common_args args) {
  std::vector<User> users;
  return users;
}

std::vector<std::string> MySQL::list_orgs() {
  std::vector<std::string> orgs;
  return orgs;
}

int64_t MySQL::count_user() {
  return 0;
}

int64_t MySQL::count_org() {
  return 0;
}

int MySQL::create_emoji(std::vector<Emoji> emojis) {
  return EXIT_SUCCESS;
}

int64_t MySQL::count_emoji() {
  return 0;
}

int64_t MySQL::count_gitignore() {
  return 0;
}

int MySQL::create_gitignore(Gitignore gitignore) {
  return EXIT_SUCCESS;
}

int64_t MySQL::count_license() {
  return 0;
}

int MySQL::create_license(License license) {
  return EXIT_SUCCESS;
}

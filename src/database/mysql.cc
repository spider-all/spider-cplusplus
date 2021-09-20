#include <database/mysql.h>

MySQL::MySQL(const std::string &host, const std::string &user,
             const std::string &password, const std::string &db_name, unsigned int port) {
  this->host = host;
  this->user = user;
  this->password = password;
  this->db_name = db_name;
  this->port = port;
}

MySQL::~MySQL() {
  if (this->client != nullptr) {
    mysql_close(this->client);
  }
  mysql_library_end();
}

int MySQL::initialize() {
  mysql_library_init(0, nullptr, nullptr);
  this->client = mysql_init(nullptr);
  if (mysql_real_connect(this->client, this->host.c_str(), this->user.c_str(), this->password.c_str(),
                         this->db_name.c_str(), this->port, nullptr, 0) == nullptr) {
    spdlog::error("Database connect with error {}", mysql_error(this->client));
  }
  const std::string create_sentence[] = {
      "CREATE TABLE IF NOT EXISTS `users` ("
      "`id` BIGINT NOT NULL PRIMARY KEY, "
      "`login` varchar(250) NOT NULL, "
      "`node_id` varchar(250) NOT NULL, "
      "`type` varchar(250) NOT NULL, "
      "`name` varchar(250) NOT NULL, "
      "`company` varchar(250), "
      "`blog` varchar(250), "
      "`location` varchar(250), "
      "`email` varchar(250), "
      "`hireable` tinyint(1), "
      "`bio` varchar(250), "
      "`created_at` varchar(250) NOT NULL, "
      "`updated_at` varchar(250) NOT NULL, "
      "`public_gists` BIGINT NOT NULL, "
      "`public_repos` BIGINT NOT NULL, "
      "`following` BIGINT NOT NULL, "
      "`followers` BIGINT NOT NULL) engine = innodb;",
      "CREATE TABLE IF NOT EXISTS `orgs` ("
      "`id` BIGINT NOT NULL PRIMARY KEY, "
      "`login` varchar(250) NOT NULL, "
      "`node_id` varchar(250) NOT NULL, "
      "`description` LONGTEXT NOT NULL) engine = innodb;",
      "CREATE TABLE IF NOT EXISTS `emojis` ("
      "`name` varchar(250) NOT NULL PRIMARY KEY, "
      "`url` varchar(250) NOT NULL) engine = innodb;",
      "CREATE TABLE IF NOT EXISTS `gitignores` ("
      "`name` varchar(250) NOT NULL PRIMARY KEY, "
      "`source` varchar(250) NOT NULL) engine = innodb;",
      "CREATE TABLE IF NOT EXISTS `licenses` ("
      "`key` varchar(250) NOT NULL PRIMARY KEY, "
      "`name` varchar(250) NOT NULL, "
      "`spdx_id` varchar(250) NOT NULL, "
      "`node_id` varchar(250) NOT NULL, "
      "`description` varchar(250), "
      "`implementation` varchar(250), "
      "`permissions` varchar(250), "
      "`conditions` varchar(250), "
      "`limitations` varchar(250), "
      "`body` varchar(250) NOT NULL, "
      "`featured` tinyint(1)) engine = innodb;",
  };

  for (const std::string &sql : create_sentence) {
    if (mysql_query(this->client, sql.c_str()) != 0) {
      spdlog::error("Execute MySQL SQL with error: {}", mysql_error(this->client));
      return DATABASE_SQL_ERROR;
    }
  }

  return EXIT_SUCCESS;
}

void BINDB(MYSQL_BIND &bind, bool *i) {
  bind.buffer_type = MYSQL_TYPE_TINY;
  bind.buffer = (char *)i;
  bind.is_unsigned = 0;
  bind.is_null = 0;
}

void BIND_int64(MYSQL_BIND &bind, int64_t *i) {
  bind.buffer_type = MYSQL_TYPE_LONG;
  bind.buffer = (char *)i;
  bind.is_unsigned = 0;
  bind.is_null = 0;
}

void BIND_int(MYSQL_BIND &bind, int *i) {
  bind.buffer_type = MYSQL_TYPE_LONG;
  bind.buffer = (char *)i;
  bind.is_unsigned = 0;
  bind.is_null = 0;
}

void BINDS(MYSQL_BIND &bind, const char *s, unsigned long *len) {
  bind.buffer_type = MYSQL_TYPE_STRING;
  bind.buffer = (char *)s;
  bind.buffer_length = *len = (unsigned long)strlen(s);
  bind.length = len;
  bind.is_null = 0;
}

template <typename F>
struct privDefer {
  F f;
  privDefer(F f) : f(f) {}
  ~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
  return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = defer_func([&]() { code; })

int MySQL::create_user(User user) {
  database_locker.lock();
  defer(database_locker.unlock());
  const std::string sql =
      "INSERT INTO `users`"
      "(`id`, `login`, `node_id`, `type`, `name`, `company`, `blog`,"
      "`location`, `email`, `hireable`, `bio`, `created_at`, `updated_at`,"
      "`public_gists`, `public_repos`, `following`, `followers`)"
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
      "ON DUPLICATE KEY UPDATE `id` = ?";

  MYSQL_STMT *stmt = mysql_stmt_init(this->client);
  if (mysql_stmt_prepare(stmt, sql.c_str(), sql.size())) {
    spdlog::error("STMT prepare SQL with error: {}", mysql_error(this->client));
    return DATABASE_SQL_ERROR;
  }
  MYSQL_BIND bind[18];
  std::memset(bind, 0, sizeof(bind));
  BIND_int64(bind[0], &user.id);
  unsigned long l0, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10;
  BINDS(bind[1], user.login.c_str(), &l0);
  BINDS(bind[2], user.node_id.c_str(), &l1);
  BINDS(bind[3], user.type.c_str(), &l2);
  BINDS(bind[4], user.name.c_str(), &l3);
  BINDS(bind[5], user.company.c_str(), &l4);
  BINDS(bind[6], user.blog.c_str(), &l5);
  BINDS(bind[7], user.location.c_str(), &l6);
  BINDS(bind[8], user.email.c_str(), &l7);
  BINDB(bind[9], &user.hireable);
  BINDS(bind[10], user.bio.c_str(), &l8);
  BINDS(bind[11], user.created_at.c_str(), &l9);
  BINDS(bind[12], user.updated_at.c_str(), &l10);
  BIND_int(bind[13], &user.public_gists);
  BIND_int(bind[14], &user.public_repos);
  BIND_int(bind[15], &user.following);
  BIND_int(bind[16], &user.followers);
  BIND_int64(bind[17], &user.id);

  if (mysql_stmt_bind_param(stmt, bind)) {
    spdlog::error("STMT bind params with error: {}", mysql_error(this->client));
    return DATABASE_SQL_ERROR;
  }

  if (mysql_stmt_execute(stmt)) {
    spdlog::error("STMT execute SQL with error: {}", mysql_error(this->client));
    return DATABASE_SQL_ERROR;
  }

  return EXIT_SUCCESS;
}

int MySQL::create_org(Org org) { return EXIT_SUCCESS; }

std::vector<std::string> MySQL::list_users() {
  database_locker.lock();
  defer(database_locker.unlock());
  const std::string sql = "select login from users order by rand() limit 100";
  if (mysql_query(this->client, sql.c_str())) {
    spdlog::error("MySQL mysql_query SQL with error: {}", mysql_error(this->client));
    return DATABASE_SQL_ERROR;
  }

  char login[250];
  unsigned long length;
  MYSQL_BIND bind_out[] = {
      {
          .buffer_type = MYSQL_TYPE_STRING,
          .buffer = login,
          .buffer_length = sizeof(login),
          .is_null = 0,
          .length = &length,
      },
  };

  MYSQL_RES *result = mysql_store_result(this->client);

  if (result == NULL) {
    finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);
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

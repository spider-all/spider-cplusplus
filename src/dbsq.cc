#include <dbsq.h>

DBSQ::DBSQ(std::string path) {
  if (sqlite3_open(path.c_str(), &db.sqlite) != SQLITE_OK) {
    spdlog::error("Open database with error: {}", sqlite3_errmsg(db.sqlite));
    code = DATABASE_OPEN_ERROR;
  }
}

int DBSQ::initialize() {
  char *zErrMsg = nullptr;
  for (auto sql : CreateSentence) {
    spdlog::info("Initialize sql: {}", sql);
    if (sqlite3_exec(db.sqlite, sql.c_str(), nullptr, nullptr, &zErrMsg) != SQLITE_OK) {
      spdlog::error("SQL error: {}", zErrMsg);
      sqlite3_free(zErrMsg);
      return DATABASE_SQL_ERROR;
    }
  }
  return EXIT_SUCCESS;
}

int DBSQ::create_user(user user) {
  sqlite3_stmt *stmt;

  std::string create_sql = "";
  std::string query_sql  = "SELECT `id` FROM `users` WHERE `id` = ?";

  int code = sqlite3_prepare_v2(db.sqlite, query_sql.c_str(), -1, &stmt, 0);
  if (code != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return code;
  }

  sqlite3_bind_int64(stmt, 1, user.id);

  int query_step = sqlite3_step(stmt);
  if (query_step == SQLITE_BUSY) {
    spdlog::error("Database error: {}", sqlite3_errstr(query_step));
    sqlite3_finalize(stmt);
    return SQLITE_BUSY;
  }
  if (query_step == SQLITE_ERROR) {
    spdlog::error("Database error: {}", sqlite3_errstr(query_step));
    sqlite3_finalize(stmt);
    return SQLITE_ERROR;
  }
  int result_count = sqlite3_data_count(stmt);
  if (result_count != 0) {
    create_sql = "UPDATE `users` SET `id` = ?, `login` = ?, `node_id` = ?, `type` = ?, `name` = ?, `company` = ?, `blog` = ?, `location` = ?, `email` = ?, `hireable` = ?, `bio` = ?, `created_at` = ?, `updated_at` = ?, `public_gists` = ?, `public_repos` = ?, `following` = ?, `followers` = ? WHERE `id` = ?";
  } else {
    create_sql = "INSERT INTO `users` VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
  }
  sqlite3_finalize(stmt);

  code = sqlite3_prepare_v2(db.sqlite, create_sql.c_str(), -1, &stmt, 0);
  if (code != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_int64(stmt, 1, user.id);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 2, user.login.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 3, user.node_id.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 4, user.type.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 5, user.name.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 6, user.company.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 7, user.blog.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 8, user.location.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 9, user.email.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_int(stmt, 10, user.hireable ? 1 : 0);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 11, user.bio.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 12, user.created_at.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_text(stmt, 13, user.updated_at.c_str(), -1, SQLITE_TRANSIENT);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_int(stmt, 14, user.public_gists);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_int(stmt, 15, user.public_repos);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_int(stmt, 16, user.following);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  code = sqlite3_bind_int(stmt, 17, user.followers);
  if (code != SQLITE_OK) {
    spdlog::error("Database error: {}", sqlite3_errstr(code));
    sqlite3_finalize(stmt);
    return code;
  }
  if (result_count != 0) {
    code = sqlite3_bind_int(stmt, 18, user.id);
    if (code != SQLITE_OK) {
      spdlog::error("Database error: {}", sqlite3_errstr(code));
      sqlite3_finalize(stmt);
      return code;
    }
  }

  int create_step = sqlite3_step(stmt);
  if (create_step == SQLITE_BUSY) {
    spdlog::error("Database error: {}", sqlite3_errstr(create_step));
    sqlite3_finalize(stmt);
    return SQLITE_BUSY;
  }
  if (create_step == SQLITE_ERROR) {
    spdlog::error("Database error: {}", sqlite3_errstr(create_step));
    sqlite3_finalize(stmt);
    return SQLITE_ERROR;
  }

  sqlite3_finalize(stmt);

  return EXIT_SUCCESS;
}

void DBSQ::deinit() {
  if (db.sqlite != nullptr) {
    sqlite3_close(db.sqlite);
  }
}

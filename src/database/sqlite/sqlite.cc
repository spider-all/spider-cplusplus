#include <database/sqlite.h>

SQLiteDatabase::SQLiteDatabase(const std::string &path) {
  this->db_path = path;
}

SQLiteDatabase::~SQLiteDatabase() {
  delete this->db;
}

int SQLiteDatabase::execute(const std::string &sql) {
  try {
    this->db->exec(sql);
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

std::string SQLiteDatabase::escape(const std::string &s) {
  std::string res;
  res.reserve(s.size());
  for (char c : s) {
    if (c == '\'') {
      res += "''";
    } else {
      res += c;
    }
  }
  return res;
}

int SQLiteDatabase::initialize() {
  this->versions = new Versions();
  try {
    this->db = new SQLite::Database(this->db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  } catch (const std::exception &e) {
    spdlog::error("SQLite open error: {}", e.what());
    return DATABASE_OPEN_ERROR;
  }
  WRAP_FUNC(this->create_collections())
  return this->initialize_version();
}

int SQLiteDatabase::initialize_version() {
  return this->versions->initialize(*this->db);
}

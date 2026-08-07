#include <database/sqlite.h>

#include <chrono>

SQLiteDatabase::SQLiteDatabase(const std::string &path) {
  this->db_path = path;
}

SQLiteDatabase::~SQLiteDatabase() {
  delete this->db;
}

int SQLiteDatabase::execute(const std::string &sql) {
  auto start = std::chrono::steady_clock::now();
  try {
    this->db->exec(sql);
  } catch (const std::exception &e) {
    auto cost = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    spdlog::error("SQLite error: {}, cost={}us, sql={}", e.what(), cost, sql);
    return SQL_EXEC_ERROR;
  }
  auto cost = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
  spdlog::debug("SQLite execute cost={}us, sql={}", cost, sql);
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
  try {
    this->db = new SQLite::Database(this->db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  } catch (const std::exception &e) {
    spdlog::error("SQLite open error: {}", e.what());
    return DATABASE_OPEN_ERROR;
  }
  return this->create_collections();
}

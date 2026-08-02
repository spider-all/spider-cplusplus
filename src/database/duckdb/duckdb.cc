#include <database/duckdb.h>

DuckDBDatabase::DuckDBDatabase(const std::string &path) {
  this->db_path = path;
}

DuckDBDatabase::~DuckDBDatabase() {
  delete this->con;
  delete this->db;
}

int DuckDBDatabase::execute(const std::string &sql) {
  try {
    auto result = this->con->Query(sql);
    if (result->HasError()) {
      spdlog::error("DuckDB error: {}", result->GetError());
      return SQL_EXEC_ERROR;
    }
  } catch (const std::exception &e) {
    spdlog::error("DuckDB error: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

std::string DuckDBDatabase::escape(const std::string &s) {
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

int DuckDBDatabase::initialize() {
  this->versions = new Versions();
  try {
    this->db = new DuckDB(this->db_path);
    this->con = new Connection(*this->db);
  } catch (const std::exception &e) {
    spdlog::error("DuckDB open error: {}", e.what());
    return DATABASE_OPEN_ERROR;
  }
  WRAP_FUNC(this->create_collections())
  return this->initialize_version();
}

int DuckDBDatabase::initialize_version() {
  return this->versions->initialize(*this->con);
}
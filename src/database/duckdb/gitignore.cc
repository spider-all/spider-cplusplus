#include <database/duckdb.h>

int DuckDBDatabase::upsert_gitignore(Gitignore gitignore) {
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO gitignores (name, source) VALUES ('{}', '{}')",
      this->escape(gitignore.name), this->escape(gitignore.source));
  return this->execute(sql);
}

int64_t DuckDBDatabase::count_gitignore() {
  return this->count_x("gitignores");
}
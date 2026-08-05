#include <database/sqlite.h>

#include <chrono>
#include <set>

#include <string_utils.h>

int64_t SQLiteDatabase::count_x(const std::string &c) {
  try {
    SQLite::Statement query(*this->db, fmt::format("SELECT COUNT(*) AS cnt FROM {}", c));
    if (query.executeStep()) {
      return query.getColumn(0).getInt64();
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
  }
  return 0;
}

int64_t SQLiteDatabase::current_timestamp() const {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

int64_t SQLiteDatabase::min_data_version(const std::string &collection) {
  try {
    SQLite::Statement query(*this->db, fmt::format("SELECT COALESCE(MIN(COALESCE(data_version, 0)), 0) FROM {}", collection));
    if (query.executeStep()) {
      return query.getColumn(0).getInt64();
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
  }
  return 1;
}

int SQLiteDatabase::update_data_version(const std::string &collection, const std::string &key_column, const std::string &key_value, int64_t version) {
  std::string sql = fmt::format(
      "UPDATE {} SET data_version = {} WHERE {} = '{}'",
      collection, version, key_column, this->escape(key_value));
  return this->execute(sql);
}

int SQLiteDatabase::upsert_relation(const std::string &collection, const std::string &first_column, int64_t first_id, const std::string &second_column, int64_t second_id) {
  int64_t now = this->current_timestamp();
  std::string sql = fmt::format(
      "INSERT INTO {} ({}, {}, data_created_at, data_updated_at, data_version) "
      "VALUES ({}, {}, {}, {}, 1) "
      "ON CONFLICT({}, {}) DO UPDATE SET "
      "data_updated_at = {}",
      collection, first_column, second_column,
      first_id, second_id, now, now,
      first_column, second_column, now);
  return this->execute(sql);
}

// list_x_random
// @params
//    keys name:string;id:int64 代表获取 name 字段类型为 string, id 字段类型为 int64 的数据
std::vector<std::string> SQLiteDatabase::list_x_random(const std::string &collection, std::string keys) {
  std::vector<std::string> result;

  std::vector<std::string> params{keys};
  std::string key = keys;
  if (string_contains(keys, KEYS_DELIMITER)) {
    key = keys.substr(0, keys.find(KEYS_DELIMITER));
    params = string_split(keys, KEYS_DELIMITER[0]);
  }

  // Build select list: extract the first part before ":" for each param
  std::vector<std::string> select_cols;
  for (const auto &param : params) {
    if (string_contains(param, VALUE_DELIMITER)) {
      select_cols.push_back(param.substr(0, param.find(VALUE_DELIMITER)));
    } else {
      select_cols.push_back(param);
    }
  }

  std::string select_expr;
  for (size_t i = 0; i < select_cols.size(); i++) {
    if (i > 0)
      select_expr += ", ";
    select_expr += fmt::format("t.{}", select_cols[i]);
  }

  std::string sql = fmt::format(
      "SELECT {} FROM {} t "
      "WHERE COALESCE(t.data_version, 0) = ("
      "SELECT COALESCE(MIN(COALESCE(data_version, 0)), 0) FROM {}) "
      "ORDER BY RANDOM() LIMIT {}",
      select_expr, collection, collection, this->sample_size);

  try {
    SQLite::Statement query(*this->db, sql);
    std::vector<std::string> selected_keys;
    while (query.executeStep()) {
      std::string res;
      bool first = true;
      for (size_t col = 0; col < select_cols.size(); col++) {
        SQLite::Column val = query.getColumn(static_cast<int>(col));
        std::string s;
        if (!val.isNull()) {
          s = val.getString();
        }
        if (first) {
          res = s;
          first = false;
        } else {
          res += KEYS_DELIMITER + s;
        }
      }
      result.push_back(res);
      selected_keys.push_back(query.getColumn(0).getString());
    }
    if (!result.empty()) {
      int64_t version = this->min_data_version(collection) + 1;
      for (const auto &selected_key : selected_keys) {
        int code = this->update_data_version(collection, select_cols[0], selected_key, version);
        if (code != EXIT_SUCCESS) {
          spdlog::error("update data version failed: collection={}, key={}, version={}", collection, selected_key, version);
          return result;
        }
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
  }
  return result;
}

int SQLiteDatabase::ensure_index(const std::string &collection, std::vector<std::string> keys) {
  std::string index_name = fmt::format("{}_index", string_join(keys, "_"));
  std::string cols = string_join(keys, ", ");
  std::string sql = fmt::format(
      "CREATE UNIQUE INDEX IF NOT EXISTS {} ON {} ({})",
      index_name, collection, cols);
  return this->execute(sql);
}

int SQLiteDatabase::create_x_collection(const std::string &collection, std::string keys) {
  if (keys.empty()) {
    return EXIT_SUCCESS;
  }
  std::vector<std::string> params{keys};
  if (string_contains(keys, KEYS_DELIMITER)) {
    params = string_split(keys, KEYS_DELIMITER[0]);
  }

  std::string columns;
  std::string pk_col;
  std::vector<std::pair<std::string, std::string>> parsed_columns;
  for (size_t i = 0; i < params.size(); i++) {
    std::string col_name = params[i];
    std::string col_type = "TEXT";
    if (string_contains(col_name, VALUE_DELIMITER)) {
      std::vector<std::string> parts;
      parts = string_split(col_name, VALUE_DELIMITER[0]);
      if (parts.size() == 2) {
        col_name = parts[0];
        std::string type_str = parts[1];
        if (type_str == "int64" || type_str == "int32" || type_str == "int") {
          col_type = "BIGINT";
        } else if (type_str == "double") {
          col_type = "DOUBLE";
        } else {
          col_type = "TEXT";
        }
      }
    }
    parsed_columns.emplace_back(col_name, col_type);
    if (i > 0)
      columns += ", ";
    columns += fmt::format("{} {}", col_name, col_type);
    if (i == 0) {
      pk_col = col_name;
    }
  }

  std::string sql = fmt::format(
      "CREATE TABLE IF NOT EXISTS {} ({}", collection, columns);
  if (!pk_col.empty()) {
    sql += fmt::format(", PRIMARY KEY ({})", pk_col);
  }
  sql += ")";

  WRAP_FUNC(this->execute(sql))

  std::set<std::string> existing_columns;
  try {
    SQLite::Statement query(*this->db, fmt::format("PRAGMA table_info({})", collection));
    while (query.executeStep()) {
      existing_columns.insert(query.getColumn(1).getString());
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
    return SQL_EXEC_ERROR;
  }

  for (const auto &[col_name, col_type] : parsed_columns) {
    if (existing_columns.contains(col_name)) {
      continue;
    }
    WRAP_FUNC(this->execute(fmt::format("ALTER TABLE {} ADD COLUMN {} {}", collection, col_name, col_type)))
  }

  return EXIT_SUCCESS;
}

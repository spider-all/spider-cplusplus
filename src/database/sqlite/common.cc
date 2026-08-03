#include <database/sqlite.h>

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

int SQLiteDatabase::update_version(std::string key, enum request_type type) {
  int64_t version = this->versions->get(type);
  if (version == 0) {
    version = 1;
  }
  std::string type_str = this->versions->to_string(type);
  spdlog::info("update version tracking: type={}({}), key={}, version={}",
               type_str, static_cast<int>(type), key, version);
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO version_tracking (type, key, version) VALUES ('{}', '{}', {})",
      this->escape(type_str), this->escape(key), version);
  return this->execute(sql);
}

int SQLiteDatabase::update_version(std::vector<std::string> keys, enum request_type type) {
  int64_t version = this->versions->get(type);
  if (version == 0) {
    version = 1;
  }
  std::string type_str = this->versions->to_string(type);
  spdlog::info("update version tracking batch: type={}({}), count={}, version={}",
               type_str, static_cast<int>(type), keys.size(), version);
  for (const auto &key : keys) {
    std::string sql = fmt::format(
        "INSERT OR REPLACE INTO version_tracking (type, key, version) VALUES ('{}', '{}', {})",
        this->escape(type_str), this->escape(key), version);
    WRAP_FUNC(this->execute(sql))
  }
  return EXIT_SUCCESS;
}

int SQLiteDatabase::incr_version(enum request_type type) {
  int64_t version = this->versions->incr(type);
  std::string type_str = this->versions->to_string(type);
  spdlog::info("increase version: type={}({}), version={}", type_str, static_cast<int>(type), version);
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO versions (type, version) VALUES ('{}', {})",
      this->escape(type_str), version);
  WRAP_FUNC(this->execute(sql))
  spdlog::info("Increase {} to {}", fmt::format("{}_version", type_str), version);
  return EXIT_SUCCESS;
}

// list_x_random
// @params
//    keys name:string;id:int64 代表获取 name 字段类型为 string, id 字段类型为 int64 的数据
std::vector<std::string> SQLiteDatabase::list_x_random(const std::string &collection, std::string keys, enum request_type type) {
  std::string type_string = this->versions->to_string(type);
  int64_t version = this->versions->get(type);
  spdlog::info("list random records: collection={}, keys={}, type={}({}), version={}",
               collection, keys, type_string, static_cast<int>(type), version);

  std::vector<std::string> result;

  std::vector<std::string> params{keys};
  std::string key = keys;
  if (boost::algorithm::contains(keys, KEYS_DELIMITER)) {
    key = keys.substr(0, keys.find(KEYS_DELIMITER));
    boost::algorithm::split(params, keys, boost::algorithm::is_any_of(KEYS_DELIMITER));
  }

  // Build select list: extract the first part before ":" for each param
  std::vector<std::string> select_cols;
  for (const auto &param : params) {
    if (boost::algorithm::contains(param, VALUE_DELIMITER)) {
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
      "SELECT {} FROM {} t LEFT JOIN version_tracking v ON v.type = '{}' AND v.key = CAST(t.{} AS TEXT) "
      "WHERE v.version IS NULL OR v.version < {} "
      "ORDER BY RANDOM() LIMIT {}",
      select_expr, collection, this->escape(type_string),
      select_cols[0], version, this->sample_size);

  try {
    SQLite::Statement query(*this->db, sql);
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
    }
    if (result.empty()) {
      spdlog::info("list random records result is empty: collection={}, type={}({}), version={}",
                   collection, type_string, static_cast<int>(type), version);
      this->incr_version(type);
    } else {
      spdlog::info("list random records result: collection={}, type={}({}), count={}",
                   collection, type_string, static_cast<int>(type), result.size());
      this->update_version(result, type);
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
  }
  return result;
}

int SQLiteDatabase::ensure_index(const std::string &collection, std::vector<std::string> keys) {
  std::string index_name = fmt::format("{}_index", boost::algorithm::join(keys, "_"));
  std::string cols = boost::algorithm::join(keys, ", ");
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
  if (boost::algorithm::contains(keys, KEYS_DELIMITER)) {
    boost::algorithm::split(params, keys, boost::algorithm::is_any_of(KEYS_DELIMITER));
  }

  std::string columns;
  std::string pk_col;
  for (size_t i = 0; i < params.size(); i++) {
    std::string col_name = params[i];
    std::string col_type = "TEXT";
    if (boost::algorithm::contains(col_name, VALUE_DELIMITER)) {
      std::vector<std::string> parts;
      boost::algorithm::split(parts, col_name, boost::algorithm::is_any_of(VALUE_DELIMITER));
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

  return this->execute(sql);
}

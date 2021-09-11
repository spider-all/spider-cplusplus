#include <config.h>

int Config::initialize(const char *config_path) {
  YAML::Node config = YAML::LoadFile(config_path);
  crawler_entry_username = config["name"].as<std::string>();
  crawler_token = config["token"].as<std::string>();
  crawler_useragent = config["useragent"].as<std::string>();
  crawler_timezone = config["timezone"].as<std::string>();

  database_path = config["database"]["path"].as<std::string>();
  database_type = config["database"]["type"].as<std::string>();
  if (database_type == DATABASE_SQLTE3) {
    database_sqlite3_path = config["database"][DATABASE_SQLTE3]["path"].as<std::string>();
  } else if (database_type == DATABASE_LEVELDB) {
    database_leveldb_path = config["database"][DATABASE_LEVELDB]["path"].as<std::string>();
  } else if (database_type == DATABASE_MONGODB) {
    database_mongodb_dsn = config["database"][DATABASE_MONGODB]["dsn"].as<std::string>();
  }
  database_host = config["database"]["host"].as<std::string>();
  database_port = config["database"]["port"].as<int>();

  if (crawler_entry_username.empty() || crawler_token.empty()) {
    spdlog::error("Config {0} have not the import value.", config_path);
    return CONFIG_PARSE_ERROR;
  }

  return 0;
}

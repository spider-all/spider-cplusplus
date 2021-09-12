#include <iostream>
#include <regex>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <const.h>
#include <error.h>

#pragma once

class Config {
private:
public:
  std::string database_type;

  std::string database_leveldb_path;

  std::string database_sqlite3_path;

  std::string database_mongodb_dsn;

  std::string database_mysql_host;
  std::string database_mysql_user;
  std::string database_mysql_password;
  std::string database_mysql_db;
  unsigned int database_mysql_port;

  std::string database_postgresql_dsn;

  std::string crawler_entry_username; // entry user name
  std::string crawler_token;          // client id
  std::string crawler_useragent;      // useragent
  std::string crawler_timezone;       // timezone
  int64_t crawler_sleep_each_request; // sleep each request

  int initialize(const std::string &config_path);
};

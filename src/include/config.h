#include <iostream>
#include <regex>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#pragma once

#include <error.hpp>

class Config {
private:
public:
  std::string database_path;
  std::string database_type;
  std::string database_host;
  int database_port;

  std::string crawler_entry_username; // entry user name
  std::string crawler_token;          // client id
  std::string crawler_useragent;      // useragent
  std::string crawler_timezone;       // timezone

  int config(char *config_path);
};

#include <iostream>
#include <regex>
#include <string>
#include <thread>

#pragma once

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <error.hpp>

class Config {
private:
public:
  std::string database_path;
  std::string database_type;
  std::string crawler_entry_username; // entry user name
  std::string crawler_client_id;      // client id
  std::string crawler_client_secret;  // client secret

  int config(char *config_path);
};

#include <iostream>
#include <regex>
#include <string>
#include <thread>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <error.hpp>

#pragma once

class Config {
private:
public:
  std::string database_path;
  std::string crawler_entry_username; // entry user name
  std::string crawler_client_id;      // client id
  std::string crawler_client_secret;  // client secret

  int config(char *config_path) {
    YAML::Node config      = YAML::LoadFile(config_path);
    crawler_entry_username = config["name"].as<std::string>();
    crawler_client_id      = config["client"]["id"].as<std::string>();
    crawler_client_secret  = config["client"]["secret"].as<std::string>();
    database_path          = config["database"].as<std::string>();

    if (crawler_entry_username == "" || crawler_client_id == "" || crawler_client_secret == "") {
      spdlog::error("Config {0} have not the import value.", config_path);
      return CONFIG_PARSE_ERROR;
    }

    return 0;
  }
};

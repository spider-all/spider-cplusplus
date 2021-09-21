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

  std::string database_mongodb_dsn;

  std::string crawler_entry_username;     // entry username
  std::vector<std::string> crawler_token; // client id
  std::string crawler_useragent;          // useragent
  std::string crawler_timezone;           // timezone
  int64_t crawler_sleep_each_request;     // sleep each request

  bool crawler_type_followers;
  bool crawler_type_followings;
  bool crawler_type_orgs;
  bool crawler_type_orgs_member;
  bool crawler_type_repos;
  bool crawler_type_emojis;
  bool crawler_type_gitignore_list;
  bool crawler_type_license_list;

  int initialize(const std::string &config_path);
};

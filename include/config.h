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
public:
  std::string database_type;

  std::string database_mongodb_dsn;

  std::string crawler_entry_username;     // entry username
  std::vector<std::string> crawler_token; // client id
  std::string crawler_useragent;          // useragent
  std::string crawler_timezone;           // timezone
  int64_t crawler_sleep_each_request;     // sleep each request

  bool crawler_type_followers = false;
  bool crawler_type_followings = false;
  bool crawler_type_orgs = false;
  bool crawler_type_orgs_member = false;
  bool crawler_type_users_repos = false;
  bool crawler_type_users_repos_branches = false;
  bool crawler_type_users_repos_branches_commits = false;
  bool crawler_type_orgs_repos = false;
  bool crawler_type_emojis = false;
  bool crawler_type_gitignore_list = false;
  bool crawler_type_license_list = false;

  bool crawler_type_trending_repos = false;
  bool crawler_type_trending_developers = false;

  int initialize(const std::string &config_path);
};

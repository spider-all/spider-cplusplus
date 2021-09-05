#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <utility>

#include <fort.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>

#include <application/application.h>
#include <database/database.h>

#include <common.h>
#include <config.h>
#include <error.hpp>

#pragma once

enum request_type {
  request_type_followers,
  request_type_following,
  request_type_user,
  request_type_orgs,
  request_type_orgs_member,
  request_type_repos,
  request_type_emoji,
  request_type_gitignore_list,
  request_type_gitignore_info,
};

class Request : public Application {
private:
  Config config;
  Database *database;

  int rate_limit_remaining{};
  int rate_limit_limit{};
  int rate_limit_reset{};

  int semaphore = 0;
  bool stopping = false;

  std::string url_host = "api.github.com";
  std::string url_prefix = "https://" + url_host;

  const std::string USERAGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.63 Safari/537.36";
  const std::string TIMEZONE = "Asia/Shanghai";

  int request_orgs_members(nlohmann::json content);
  int request_orgs(nlohmann::json content);
  int request_user(nlohmann::json content);
  int request_followx(nlohmann::json content);
  int request_emoji(nlohmann::json content);
  int request_gitignore_list(nlohmann::json content);
  int request_gitignore_info(nlohmann::json content);

public:
  Request(Config, Database *);
  ~Request() override;

  int startup() override;

  int request(const std::string &url, enum request_type type);
};

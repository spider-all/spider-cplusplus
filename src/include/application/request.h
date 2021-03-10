#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <utility>

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
  request_type_userinfo,
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

  const std::string USERAGENT =
      "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_3) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/88.0.4324.192 Safari/537.36";
  const std::string TIMEZONE = "Asia/Shanghai";

public:
  Request(Config, Database *);
  ~Request() override;

  int startup() override;

  int request(const std::string &url, enum request_type type);
};

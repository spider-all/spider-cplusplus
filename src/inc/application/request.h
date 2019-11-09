#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>

#pragma once

#include <application/application.h>
#include <common.h>
#include <config.h>
#include <database/database.h>
#include <error.hpp>

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

  int semaphore = 0; // 执行过程中的信号量
  bool stopping = false;

  std::string url_host   = "api.github.com";
  std::string url_prefix = "https://" + url_host;

  const std::string USERAGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36";
  const std::string TIMEZONE  = "Asia/Shanghai";

  static size_t body_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
  }

  static size_t header_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
  }

public:
  Request(Config, Database *);
  ~Request() override;

  int startup() override;

  int request(const std::string& url, enum request_type type);
};

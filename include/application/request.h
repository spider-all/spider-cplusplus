#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <utility>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <application.h>
#include <database.h>

#include <common.h>
#include <config.h>
#include <error.h>
#include <model.h>

#pragma once

typedef struct ExtraData {
  int64_t user_id = 0;
} ExtraData;

typedef struct TrendingData {
  std::string seq;
  std::string spoken_language;
  std::string language;
} TrendingData;

typedef struct RequestConfig {
  std::string host;
  std::string path;
  ExtraData extra;
  TrendingData trending;
  std::string response_type;
} RequestConfig;

#define REQUEST_CONFIG(url) RequestConfig{ \
    .host = this->default_url_prefix,      \
    .path = request_url,                   \
};

class Request : public Application {
private:
  Config config;
  Database *database;

  int semaphore = 0;
  bool stopping = false;

  std::string url_host = "api.github.com";
  std::string default_url_prefix = "https://" + url_host;

  const std::string USERAGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.63 Safari/537.36";
  const std::string TIMEZONE = "Asia/Shanghai";

  int startup_followx();
  int startup_info();
  int startup_orgs();
  int startup_xrepos();
  int startup_events();

  int request_orgs_members(const nlohmann::json &content, enum request_type type_from);
  int request_orgs(const nlohmann::json &content, enum request_type type_from);
  int request_user(nlohmann::json content, enum request_type type_from);
  int request_followx(const nlohmann::json &content, enum request_type type, enum request_type type_from, const ExtraData &extra);
  int request_repo_list(nlohmann::json content, enum request_type type_from);
  int request_starred(nlohmann::json content, const ExtraData &extra);
  int request_events(const nlohmann::json &content);
  int request_trending_repos(const nlohmann::json &content);

public:
  Request(Config, Database *);
  ~Request() override;

  int startup() override;

  // skip_sleep skips the configured per-request sleep.
  int request(RequestConfig &url, enum request_type type, enum request_type type_from, bool skip_sleep = false);
};

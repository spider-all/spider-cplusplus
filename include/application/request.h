#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <fort.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>

#include <gumbo.h>

#include <application.h>
#include <database.h>

#include <common.h>
#include <config.h>
#include <error.h>
#include <model.h>

#pragma once

typedef struct ExtralData {
  std::string user;
  std::string repo;
  std::string branch;
} ExtralData;

typedef struct TrendingData {
  std::string seq;
  std::string spoken_language;
  std::string language;
} TrendingData;

typedef struct RequestConfig {
  std::string host;
  std::string path;
  ExtralData extral;
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

  int rate_limit_remaining{};
  int rate_limit_limit{};
  int rate_limit_reset{};

  std::mutex request_locker;

  unsigned long token_index = 0;

  int semaphore = 0;
  bool stopping = false;

  std::string url_host = "api.github.com";
  std::string default_url_prefix = "https://" + url_host;

  const std::string USERAGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.63 Safari/537.36";
  const std::string TIMEZONE = "Asia/Shanghai";

  int startup_followx();
  int startup_info();
  int startup_emojis();
  int startup_orgs();
  int startup_gitignore();
  int startup_license();
  int startup_xrepos();
  int startup_repos_branches();
  int startup_repos_branches_commits();

  int startup_trending_repos();

  int request_orgs_members(const nlohmann::json &content, enum request_type type_from);
  int request_orgs(const nlohmann::json &content, enum request_type type_from);
  int request_user(nlohmann::json content, enum request_type type_from);
  int request_followx(const nlohmann::json &content, enum request_type type_from);
  int request_emoji(nlohmann::json content, enum request_type type_from);
  int request_gitignore_list(const nlohmann::json &content, enum request_type type_from);
  int request_gitignore_info(nlohmann::json content, enum request_type type_from);
  int request_license_list(const nlohmann::json &content, enum request_type type_from);
  int request_license_info(nlohmann::json content, enum request_type type_from);
  int request_repo_list(nlohmann::json content, enum request_type type_from);
  int request_repo_branches(nlohmann::json content, ExtralData extral, enum request_type type_from);

  int search_for_article(GumboNode *node, const TrendingData &trending);

public:
  Request(Config, Database *);
  ~Request() override;

  int startup() override;

  // skip_sleep is used to skip sleep when requesting, but firstly we need to skip sleep when requesting
  int request(RequestConfig &url, enum request_type type, enum request_type type_from, bool skip_sleep = false);
};

#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>

#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <curl/curl.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>

#include <application.h>
#include <common.h>
#include <config.h>
#include <database.h>
#include <error.hpp>

enum request_type {
  request_type_followers,
  request_type_userinfo,
};

class Request : public Application {
private:
  Config config;
  Database *database;

  int semaphore = 0; // 执行过程中的信号量
  bool stopping = false;

  std::string url_prefix = "api.github.com";

  std::string url_suffix;

  const std::string USERAGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_2) "
                                "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36";
  static size_t body_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
  }

  static size_t header_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
  }

public:
  Request(Config c, Database *db);

  int startup();

  void teardown();

  int request(std::string url, enum request_type type);
};

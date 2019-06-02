#include <ctime>
#include <iostream>
#include <regex>
#include <string>
#include <thread>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>

#include <common.hpp>
#include <config.hpp>
#include <database.hpp>
#include <error.hpp>
#include <model.hpp>

#pragma once

class Request {
private:
  Config config;
  Database database;

  int semaphore = 0; // 执行过程中的信号量
  bool stopping = false;

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
  Request(Config c, Database db) {
    config     = c;
    database   = db;
    url_suffix = "?client_id=" + config.crawler_client_id + "&client_secret=" + config.crawler_client_secret;
  }

  enum request_type {
    request_type_followers,
    request_type_userinfo,
  };

  int startup() {
    spdlog::info("Spider is running...");
    int code = request("https://api.github.com/users/" + config.crawler_entry_username + url_suffix, request_type_userinfo);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }

    std::thread followers_thread([=]() {
      semaphore++;
      while (!stopping) {

        std::string request_url = "https://api.github.com/users/" + config.crawler_entry_username + "/followers" + url_suffix + "&page=1";

        int code = request(request_url, request_type_followers);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
      }
      semaphore--;
    });
    followers_thread.detach();
    return 0;
  }

  void teardown() {
    stopping = true;
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
      if (semaphore == 0) {
        break;
      }
    }

    spdlog::info("Spider running over...");
  }

  int request(std::string url, enum request_type type) {
    CURL *curl = curl_easy_init();
    std::string body, header;

    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, body_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
      curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
      curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
      CURLcode res = curl_easy_perform(curl);
      if (res != CURLE_OK) {
        spdlog::error("Request with error: {}", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return REQUEST_ERROR;
      }
      curl_easy_cleanup(curl);
    }

    if (body == "") {
      return REQUEST_ERROR;
    }

    nlohmann::json content;

    try {
      nlohmann::json::parser_callback_t cb = [](int /*depth*/, nlohmann::json::parse_event_t event, nlohmann::json &parsed) {
        if (event == nlohmann::json::parse_event_t::key) {
          std::string str = parsed.dump();
          str.erase(str.begin(), str.begin() + 1);
          str.erase(str.end() - 1, str.end());
          if (end_with(str, "_url") or str == "url") {
            return false;
          }
        } else if (event == nlohmann::json::parse_event_t::value && parsed.dump() == "null") {
          parsed = nlohmann::json("");
          return true;
        }
        return true;
      };
      content = nlohmann::json::parse(body, cb);
    } catch (nlohmann::detail::parse_error &e) {
      spdlog::error("Request {} got error: {}", url.c_str(), e.what());
      return REQUEST_ERROR;
    }

    github_user user;
    int code = 0;

    switch (type) {
    case request_type_followers:
      for (auto i : content) {
        code = request("https://api.github.com/users/" + i["login"].get<std::string>() + url_suffix, request_type_userinfo);
        if (code != 0) {
          spdlog::error("Request userinfo with error: {}", code);
        }
        if (stopping) {
          return EXIT_SUCCESS;
        }
      }
      break;
    case request_type_userinfo:
      if (content["hireable"].dump() != "true") {
        content["hireable"] = false;
      }
      user.login        = content["login"].get<std::string>();
      user.id           = content["id"].get<int64_t>();
      user.node_id      = content["node_id"].get<std::string>();
      user.type         = content["type"].get<std::string>();
      user.name         = content["name"].get<std::string>();
      user.company      = content["company"].get<std::string>();
      user.blog         = content["blog"].get<std::string>();
      user.location     = content["location"].get<std::string>();
      user.email        = content["email"].get<std::string>();
      user.hireable     = content["hireable"].get<bool>();
      user.bio          = content["bio"].get<std::string>();
      user.created_at   = content["created_at"].get<std::string>();
      user.updated_at   = content["updated_at"].get<std::string>();
      user.public_gists = content["public_gists"].get<int>();
      user.public_repos = content["public_repos"].get<int>();
      user.following    = content["following"].get<int>();
      user.followers    = content["followers"].get<int>();

      code = database.create_user(user);
      if (code != 0) {
        spdlog::error("Database with error: {}", code);
      }
      break;
    default:
      return UNKNOWN_REQUEST_TYPE;
    }

    std::regex pieces_regex("<(https:\\/\\/api\\.github\\.com\\/[0-9a-z\\/\\?_=&]+)>;\\srel=\"(next|last|prev|first)\"");
    std::smatch result;
    while (regex_search(header, result, pieces_regex)) {
      if (result.size() == 3 && result[2] == "next") {
        return request(result[1], type);
      }
      header = result.suffix().str();
    };

    return 0;
  }
};

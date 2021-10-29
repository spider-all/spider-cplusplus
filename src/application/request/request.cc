#include <application/request.h>

Request::Request(Config c, Database *db) {
  config = std::move(c);
  database = db;
}

Request::~Request() {
  stopping = true;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
    if (semaphore == 0) {
      break;
    }
  }

  spdlog::info("Spider stopped...");
}

int Request::startup() {
  spdlog::info("Spider is running...");
  std::string request_url = "/users/" + config.crawler_entry_username;

  int code = request(request_url, request_type_user, request_type_followers, true);
  if (code != 0) {
    return code;
  }
  if (stopping) {
    return EXIT_SUCCESS;
  }

  this->startup_followx();
  this->startup_info();
  this->startup_emojis();
  this->startup_orgs();
  this->startup_gitignore();
  this->startup_license();
  this->startup_xrepos();

  return EXIT_SUCCESS;
}

int Request::request(const std::string &url, enum request_type type, enum request_type type_from, bool skip_sleep) {
  if (stopping) {
    return EXIT_SUCCESS;
  }

  spdlog::info("Crawler url: {}", url);

  std::string _useragent = USERAGENT;
  if (!config.crawler_useragent.empty()) {
    _useragent = config.crawler_useragent;
  }
  std::string _timezone = TIMEZONE;
  if (!config.crawler_timezone.empty()) {
    _timezone = config.crawler_timezone;
  }

  httplib::Client client(url_prefix.c_str());
  httplib::Headers headers = {
      {"Accept", "application/json"},
      {"Host", url_host},
      {"User-Agent", _useragent},
      {"Time-Zone", _timezone},
      {"Authorization", "Bearer " + config.crawler_token[token_index]},
  };

  request_locker.lock();
  httplib::Result response(nullptr, httplib::Unknown, httplib::Headers{});
  try {
    response = client.Get(url.c_str(), headers);
  } catch (const std::exception &e) {
    request_locker.unlock();
    spdlog::error("Request with error: {}, {}", url, e.what());
    return REQUEST_ERROR;
  }
  request_locker.unlock();

  if (response == nullptr) {
    spdlog::error("Request with error: {}", url);
    return REQUEST_ERROR;
  }

  for (const auto &header : response->headers) {
    if (header.first == "X-RateLimit-Limit") {
      rate_limit_limit = std::stoi(header.second, nullptr);
    } else if (header.first == "X-RateLimit-Reset") {
      rate_limit_reset = std::stoi(header.second);
    } else if (header.first == "X-RateLimit-Remaining") {
      rate_limit_remaining = std::stoi(header.second);
    }
  }

  if (rate_limit_remaining % 10 == 0) {
    spdlog::info("Rate limit: {}/{}", rate_limit_remaining, rate_limit_limit);
    std::time_t result = rate_limit_reset;
    char buffer[32];
    std::strftime(buffer, 32, "%Y/%m/%d %H:%M:%S", std::localtime(&result));
    spdlog::info("Rate limit reset at: {}", buffer);
  }

  if (response->status == 403) {
    auto now = std::chrono::system_clock::now();
    auto current = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    spdlog::info("Wait for another {}s to request due to rate limit, X-RateLimit-Reset: {}", rate_limit_reset - current, rate_limit_reset);
    spdlog::info("Change token to next and retry");
    token_index++;
    token_index = token_index % config.crawler_token.size();

    return request(url, type, type_from);
  }

  if (response->status != 200) {
    spdlog::error("Got {} on request url: {}, {}", response->status, url, response->body);
    return REQUEST_ERROR;
  }

  if (response->body.empty()) {
    return REQUEST_ERROR;
  }

  nlohmann::json content;

  try {
    nlohmann::json::parser_callback_t cb =
        [=](int /*depth*/, nlohmann::json::parse_event_t event, nlohmann::json &parsed) {
          if (event == nlohmann::json::parse_event_t::key) {
            std::string str = parsed.dump();
            str.erase(str.begin(), str.begin() + 1);
            str.erase(str.end() - 1, str.end());
            if (Common::end_with(str, "_url") or str == "url") {
              return false;
            }
          } else if (event == nlohmann::json::parse_event_t::value && parsed.dump() == "null") {
            parsed = nlohmann::json("");
            return true;
          }
          return true;
        };
    content = nlohmann::json::parse(response->body, cb);
  } catch (nlohmann::detail::parse_error &e) {
    spdlog::error("Request {} got error: {}", url.c_str(), e.what());
    return REQUEST_ERROR;
  }

  int code;
  switch (type) {
  case request_type_following:
  case request_type_followers:
    code = request_followx(content, type_from);
    if (code != 0) {
      spdlog::error("Request userinfo with error: {}", code);
    }
    break;
  case request_type_orgs:
    code = request_orgs(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_orgs_member:
    code = request_orgs_members(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_user:
    code = request_user(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_emoji:
    code = request_emoji(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_gitignore_list:
    code = request_gitignore_list(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_gitignore_info:
    code = request_gitignore_info(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_license_list:
    code = request_license_list(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_license_info:
    code = request_license_info(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
  case request_type_orgs_repos:
  case request_type_users_repos:
    code = this->request_repo_list(content, type_from);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  default:
    return UNKNOWN_REQUEST_TYPE;
  }

  if (!skip_sleep) {
    std::this_thread::sleep_for(std::chrono::milliseconds(config.crawler_sleep_each_request));
  }

  std::regex pieces_regex(R"lit(<(https:\/\/api\.github\.com\/[0-9a-z\/\?_=&]+)>;\srel="(next|last|prev|first)")lit");
  std::smatch result;
  std::string header_link;
  httplib::Headers::iterator it = response->headers.find("Link");
  if (response->headers.end() != it && !it->second.empty()) {
    header_link = it->second;
  }
  while (!header_link.empty() && regex_search(header_link, result, pieces_regex)) {
    if (result.size() == 3 && result[2] == "next") {
      auto u = std::string(result[1]);
      size_t pos = u.find(url_prefix);
      if (pos != std::string::npos) {
        u.erase(pos, url_prefix.length());
      }

      return request(u, type, type_from);
    }
    header_link = result.suffix().str();
  }

  return EXIT_SUCCESS;
}

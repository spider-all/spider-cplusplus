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

  SPDLOG_INFO("Spider stopped...");
}

int Request::startup() {
  SPDLOG_INFO("Spider is running...");
  RequestConfig request_config{
      .host = this->default_url_prefix,
      .path = "/users/" + config.crawler_entry_username,
  };
  WRAP_FUNC(request(request_config, request_type_user, request_type_followers, true))

  if (stopping) {
    return EXIT_SUCCESS;
  }

  WRAP_FUNC(this->startup_followx())
  WRAP_FUNC(this->startup_info())
  WRAP_FUNC(this->startup_emojis())
  WRAP_FUNC(this->startup_orgs())
  WRAP_FUNC(this->startup_gitignore())
  WRAP_FUNC(this->startup_license())
  WRAP_FUNC(this->startup_xrepos())
  WRAP_FUNC(this->startup_repos_branches())
  WRAP_FUNC(this->startup_repos_branches_commits())

  return EXIT_SUCCESS;
}

int Request::request(RequestConfig &request_config, enum request_type type, enum request_type type_from, bool skip_sleep) {
  if (this->stopping) {
    return EXIT_SUCCESS;
  }

  SPDLOG_INFO("Crawler url: {}{}", request_config.host, request_config.path);

  std::string _useragent = USERAGENT;
  if (!config.crawler_useragent.empty()) {
    _useragent = this->config.crawler_useragent;
  }
  std::string _timezone = TIMEZONE;
  if (!config.crawler_timezone.empty()) {
    _timezone = this->config.crawler_timezone;
  }

  std::string url_prefix = request_config.host;
  if (url_prefix.empty()) {
    url_prefix = this->default_url_prefix;
  }

  std::string header_host = boost::algorithm::trim_left_copy_if(url_prefix, boost::is_any_of("https://"));
  httplib::Client client(url_prefix.c_str());
  httplib::Headers headers = {
      {"Host", header_host},
      {"User-Agent", _useragent},
      {"Time-Zone", _timezone},
      {"Authorization", "Bearer " + config.crawler_token[token_index]},
  };

  if (request_config.response_type == "" || request_config.response_type == "json") {
    headers.insert(std::make_pair("Accept", "application/json"));
  }

  if (!skip_sleep) {
    std::time_t now = std::time(0);
    boost::random::mt19937 gen{static_cast<std::uint16_t>(now)};
    boost::random::uniform_int_distribution<> sleep_random{0, static_cast<int>(config.crawler_sleep_each_request)};
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_random(gen)));
  }

  this->request_locker.lock();
  if (this->stopping) {
    return EXIT_SUCCESS;
  }
  httplib::Result response(nullptr, httplib::Error::Unknown, httplib::Headers{});
  try {
    response = client.Get(request_config.path.c_str(), headers);
  } catch (const std::exception &e) {
    this->request_locker.unlock();
    spdlog::error("Request with error: {}, {}", request_config.path, e.what());
    return REQUEST_ERROR;
  }
  this->request_locker.unlock();

  if (this->stopping) {
    return EXIT_SUCCESS;
  }

  if (response == nullptr) {
    spdlog::error("Request with error: {}", request_config.path);
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
    SPDLOG_INFO("Rate limit: {}/{}", rate_limit_remaining, rate_limit_limit);
    std::time_t result = rate_limit_reset;
    char buffer[32];
    std::strftime(buffer, 32, "%Y/%m/%d %H:%M:%S", std::localtime(&result));
    SPDLOG_INFO("Rate limit reset at: {}", buffer);
  }

  if (response->status == 403) {
    auto now = std::chrono::system_clock::now();
    auto current = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    SPDLOG_INFO("Wait for another {}s to request due to rate limit, X-RateLimit-Reset: {}", rate_limit_reset - current, rate_limit_reset);
    SPDLOG_INFO("Change token to next and retry");
    token_index++;
    token_index = token_index % config.crawler_token.size();
    std::this_thread::sleep_for(std::chrono::milliseconds(this->sleep_for_another_token));
    this->sleep_for_another_token *= 2;
    if (this->sleep_for_another_token >= 30 * 1000 * 60 /* 30min */) {
      this->sleep_for_another_token = 30 * 1000 * 60;
    }
    return request(request_config, type, type_from);
  }
  this->sleep_for_another_token = 1000;

  if (response->status != 200) {
    spdlog::error("Got {} on request url: {}{}, {}", response->status, request_config.host, request_config.path, response->body);
    return REQUEST_ERROR;
  }

  if (response->body.empty()) {
    return REQUEST_ERROR;
  }

  if (request_config.response_type == "" || request_config.response_type == "json") {
    nlohmann::json content;
    try {
      content = nlohmann::json::parse(response->body);
    } catch (const std::exception &e) {
      spdlog::error("Parse json with error: {}, {}", request_config.path, e.what());
      return REQUEST_ERROR;
    }

    try {
      nlohmann::json::parser_callback_t cb =
          [=](int /*depth*/, nlohmann::json::parse_event_t event, nlohmann::json &parsed) {
            if (event == nlohmann::json::parse_event_t::key) {
              std::string str = parsed.dump();
              str.erase(str.begin(), str.begin() + 1);
              str.erase(str.end() - 1, str.end());
              if (boost::algorithm::ends_with(str, "_url") or str == "url") {
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
      spdlog::error("Request {} got error: {}", request_config.path, e.what());
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
    case request_type_users_repos_branches:
      code = this->request_repo_branches(content, request_config.extral, type_from);
      if (code != 0) {
        spdlog::error("Database with error: {}", code);
      }
      break;
    default:
      SPDLOG_INFO("Unknown request type: {}", type);
      return UNKNOWN_REQUEST_TYPE;
    }
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
      request_config.path = u;
      return request(request_config, type, type_from);
    }
    header_link = result.suffix().str();
  }

  return EXIT_SUCCESS;
}

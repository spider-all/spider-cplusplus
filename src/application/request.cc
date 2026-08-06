#include <application/request.h>

#include <curl/curl.h>
#include <fmt/core.h>

#include <map>
#include <random>

#include <string_utils.h>

namespace {
struct HttpResponse {
  long status = 0;
  std::string body;
  std::map<std::string, std::string> headers;
};

size_t write_body(char *ptr, size_t size, size_t nmemb, void *userdata) {
  auto *body = static_cast<std::string *>(userdata);
  body->append(ptr, size * nmemb);
  return size * nmemb;
}

size_t write_header(char *buffer, size_t size, size_t nitems, void *userdata) {
  size_t total = size * nitems;
  std::string header(buffer, total);
  size_t pos = header.find(':');
  if (pos == std::string::npos) {
    return total;
  }

  std::string key = header.substr(0, pos);
  std::string value = header.substr(pos + 1);
  string_trim(key);
  string_trim(value);
  string_to_lower(key);

  auto *headers = static_cast<std::map<std::string, std::string> *>(userdata);
  (*headers)[key] = value;
  return total;
}
} // namespace

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

  spdlog::info("spider stopped...");
}

int Request::startup() {
  spdlog::info("spider is running...");
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
  WRAP_FUNC(this->startup_orgs())
  WRAP_FUNC(this->startup_xrepos())
  WRAP_FUNC(this->startup_events())

  return EXIT_SUCCESS;
}

int Request::request(RequestConfig &request_config, enum request_type type, enum request_type type_from, bool skip_sleep) {
  if (this->stopping) {
    return EXIT_SUCCESS;
  }

  spdlog::info("crawler request: type={}({}), type_from={}({}), url={}{}",
               request_type_name(type), static_cast<int>(type),
               request_type_name(type_from), static_cast<int>(type_from),
               request_config.host, request_config.path);

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

  std::string header_host = url_prefix;
  if (header_host.starts_with("https://")) {
    header_host.erase(0, std::string("https://").size());
  }

  if (!skip_sleep) {
    std::time_t now = std::time(0);
    std::mt19937 gen{static_cast<std::uint32_t>(now)};
    std::uniform_int_distribution<> sleep_random{0, static_cast<int>(config.crawler_sleep_each_request)};
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_random(gen)));
  }

  this->request_locker.lock();
  if (this->stopping) {
    this->request_locker.unlock();
    return EXIT_SUCCESS;
  }

  HttpResponse response;
  CURL *curl = curl_easy_init();
  if (curl == nullptr) {
    this->request_locker.unlock();
    spdlog::error("init curl with error: {}", request_config.path);
    return REQUEST_ERROR;
  }

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, fmt::format("Host: {}", header_host).c_str());
  headers = curl_slist_append(headers, fmt::format("User-Agent: {}", _useragent).c_str());
  headers = curl_slist_append(headers, fmt::format("Time-Zone: {}", _timezone).c_str());
  headers = curl_slist_append(headers, fmt::format("Authorization: Bearer {}", config.crawler_token[token_index]).c_str());
  if (request_config.response_type == "" || request_config.response_type == "json") {
    headers = curl_slist_append(headers, "Accept: application/json");
  }

  std::string request_url = url_prefix + request_config.path;
  try {
    curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
      this->request_locker.unlock();
      spdlog::error("request with error: {}, {}", request_config.path, curl_easy_strerror(result));
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
      return REQUEST_ERROR;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);
  } catch (const std::exception &e) {
    this->request_locker.unlock();
    spdlog::error("request with error: {}, {}", request_config.path, e.what());
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return REQUEST_ERROR;
  }
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  this->request_locker.unlock();

  if (this->stopping) {
    return EXIT_SUCCESS;
  }

  for (const auto &header : response.headers) {
    if (header.first == "x-ratelimit-limit") {
      rate_limit_limit = std::stoi(header.second, nullptr);
    } else if (header.first == "x-ratelimit-reset") {
      rate_limit_reset = std::stoi(header.second);
    } else if (header.first == "x-ratelimit-remaining") {
      rate_limit_remaining = std::stoi(header.second);
    }
  }

  if (rate_limit_remaining % 10 == 0) {
    std::time_t result = rate_limit_reset;
    char buffer[32];
    std::strftime(buffer, 32, "%Y/%m/%d %H:%M:%S", std::localtime(&result));
    spdlog::info("rate limit: {}/{}, reset at: {}", rate_limit_remaining, rate_limit_limit, buffer);
  }

  if (response.status == 403) {
    auto now = std::chrono::system_clock::now();
    auto current = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    spdlog::info("wait for another {}s to request due to rate limit, X-RateLimit-Reset: {}", rate_limit_reset - current, rate_limit_reset);
    spdlog::info("Change token to next and retry");
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

  if (response.status != 200) {
    spdlog::error("Got {} on request url: {}{}, {}", response.status, request_config.host, request_config.path, response.body);
    return REQUEST_ERROR;
  }

  if (response.body.empty()) {
    return REQUEST_ERROR;
  }

  if (request_config.response_type == "" || request_config.response_type == "json") {
    nlohmann::json content;
    try {
      content = nlohmann::json::parse(response.body);
    } catch (const std::exception &e) {
      spdlog::error("parse json with error: {}, {}", request_config.path, e.what());
      return REQUEST_ERROR;
    }

    try {
      nlohmann::json::parser_callback_t cb =
          [=](int /*depth*/, nlohmann::json::parse_event_t event, nlohmann::json &parsed) {
            if (event == nlohmann::json::parse_event_t::key) {
              std::string str = parsed.dump();
              str.erase(str.begin(), str.begin() + 1);
              str.erase(str.end() - 1, str.end());
              if (string_ends_with(str, "_url") or str == "url") {
                return false;
              }
            } else if (event == nlohmann::json::parse_event_t::value && parsed.dump() == "null") {
              parsed = nlohmann::json("");
              return true;
            }
            return true;
          };
      content = nlohmann::json::parse(response.body, cb);
    } catch (nlohmann::detail::parse_error &e) {
      spdlog::error("Request {} got error: {}", request_config.path, e.what());
      return REQUEST_ERROR;
    }

    int code;
    try {
      switch (type) {
      case request_type_following:
      case request_type_followers:
        code = request_followx(content, type, type_from, request_config.extra);
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
      case request_type_orgs_repos:
      case request_type_users_repos:
      case request_type_config_repos:
        code = this->request_repo_list(content, type_from);
        if (code != 0) {
          spdlog::error("Database with error: {}", code);
        }
        break;
      case request_type_starred:
        code = this->request_starred(content, request_config.extra);
        if (code != 0) {
          spdlog::error("Database with error: {}", code);
        }
        break;
      case request_type_events:
        code = this->request_events(content);
        if (code != 0) {
          spdlog::error("Database with error: {}", code);
        }
        break;
      default:
        spdlog::error("unknown request type in Request::request: type={}({}), type_from={}({}), path={}",
                      request_type_name(type), static_cast<int>(type),
                      request_type_name(type_from), static_cast<int>(type_from),
                      request_config.path);
        return UNKNOWN_REQUEST_TYPE;
      }
    } catch (const nlohmann::json::type_error &e) {
      spdlog::error("json error on {}: type={}({}), type_from={}({}), error={}, body={}",
                    request_config.path,
                    request_type_name(type), static_cast<int>(type),
                    request_type_name(type_from), static_cast<int>(type_from),
                    e.what(), response.body);
      return REQUEST_ERROR;
    }
  }

  std::regex pieces_regex(R"lit(<(https:\/\/api\.github\.com\/[0-9a-z\/\?_=&]+)>;\srel="(next|last|prev|first)")lit");
  std::smatch result;
  std::string header_link;
  auto it = response.headers.find("link");
  if (response.headers.end() != it && !it->second.empty()) {
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

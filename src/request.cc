#include <request.h>

Request::Request(Config c, Database *db) {
  config   = c;
  database = db;

  url_suffix = "?client_id=" + config.crawler_client_id + "&client_secret=" + config.crawler_client_secret;
}

int Request::startup() {
  spdlog::info("Spider is running...");
  int code = request("/users/" + config.crawler_entry_username + url_suffix, request_type_userinfo);
  if (code != 0) {
    return code;
  }
  if (stopping) {
    return EXIT_SUCCESS;
  }

  std::thread followers_thread([=]() {
    semaphore++;
    while (!stopping) {

      std::string request_url = "/users/" + config.crawler_entry_username + "/followers" + url_suffix + "&page=1";

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

void Request::teardown() {
  stopping = true;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
    if (semaphore == 0) {
      break;
    }
  }

  spdlog::info("Spider running over...");
}

int Request::request(std::string url, enum request_type type) {
  httplib::SSLClient client(url_prefix.c_str());
  client.set_ca_cert_path("ca-bundle.crt");
  client.enable_server_certificate_verification(true);
  std::shared_ptr<httplib::Response> res = client.Get(url.c_str(), headers);

  httplib::Headers::iterator rate_limit_remaining_iter = res->headers.find("X-RateLimit-Limit");
  if (rate_limit_remaining_iter != res->headers.end()) {
    rate_limit_remaining = std::stoi(rate_limit_remaining_iter->second);
  }

  httplib::Headers::iterator rate_limit_limit_iter = res->headers.find("X-RateLimit-Remaining");
  if (rate_limit_limit_iter != res->headers.end()) {
    rate_limit_limit = std::stoi(rate_limit_limit_iter->second);
  }

  httplib::Headers::iterator rate_limit_reset_iter = res->headers.find("X-RateLimit-Reset");
  if (rate_limit_reset_iter != res->headers.end()) {
    rate_limit_reset = std::stoi(rate_limit_reset_iter->second);
  }

  if (res->status == 403) {
    std::this_thread::sleep_for(std::chrono::seconds(30));
    spdlog::info("Wait for another 30s to request due to rate limit, X-RateLimit-Reset: {}", rate_limit_reset);
    return request(url, type);
  }

  if (res->status != 200) {
    spdlog::error("Got {} on request url: {}", res->status, url);
    return EXIT_FAILURE;
  }

  std::string header;
  httplib::Headers::iterator iter = res->headers.find("Link");
  if (iter != res->headers.end()) {
    header = iter->second;
  }

  if (res->body == "") {
    return REQUEST_ERROR;
  }

  nlohmann::json content;

  try {
    nlohmann::json::parser_callback_t cb = [=](int /*depth*/, nlohmann::json::parse_event_t event, nlohmann::json &parsed) {
      if (event == nlohmann::json::parse_event_t::key) {
        std::string str = parsed.dump();
        str.erase(str.begin(), str.begin() + 1);
        str.erase(str.end() - 1, str.end());
        if (common.end_with(str, "_url") or str == "url") {
          return false;
        }
      } else if (event == nlohmann::json::parse_event_t::value && parsed.dump() == "null") {
        parsed = nlohmann::json("");
        return true;
      }
      return true;
    };
    content = nlohmann::json::parse(res->body, cb);
  } catch (nlohmann::detail::parse_error &e) {
    spdlog::error("Request {} got error: {}", url.c_str(), e.what());
    return REQUEST_ERROR;
  }

  user user;
  int code = 0;

  switch (type) {
  case request_type_followers:
    for (auto i : content) {
      code = request("/users/" + i["login"].get<std::string>() + url_suffix, request_type_userinfo);
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

    code = database->create_user(user);
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

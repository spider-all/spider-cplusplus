#include <application/request.h>

Request::Request(Config c, Database *db) {
  config   = c;
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

  spdlog::info("Spider running over...");
}

int Request::startup() {
  spdlog::info("Spider is running...");
  std::string request_url = url_prefix + "/users/" + config.crawler_entry_username;

  int code = request(request_url, request_type_userinfo);
  if (code != 0) {
    return code;
  }
  if (stopping) {
    return EXIT_SUCCESS;
  }

  semaphore++;
  std::thread followers_thread([=]() {
    spdlog::info("Followers thread starting...");
    while (!stopping) {
      std::vector<std::string> users = database->list_users();
      for (std::string u : users) {

        std::string request_url = url_prefix + "/users/" + u + "/followers";

        int code = request(request_url, request_type_followers);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
        if (stopping) {
          break;
        }
      }
    }
    spdlog::info("Followers thread stopped");
    semaphore--;
  });
  followers_thread.detach();

  semaphore++;
  std::thread followings_thread([=]() {
    spdlog::info("Following thread starting...");
    while (!stopping) {
      std::vector<std::string> users = database->list_users();
      for (std::string u : users) {
        std::string request_url = url_prefix + "/users/" + u + "/following";

        int code = request(request_url, request_type_following);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
        if (stopping) {
          break;
        }
      }
    }
    spdlog::info("Following thread stopped");
    semaphore--;
  });
  followings_thread.detach();

  semaphore++;
  std::thread info_thread([=]() {
    spdlog::info("Info thread starting...");
    while (!stopping) {
      std::vector<std::string> users = database->list_users();
      spdlog::info("Database have users: {}", users.size());
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("Info thread stopped");
    semaphore--;
  });
  info_thread.detach();

  return 0;
}

int Request::request(std::string url, enum request_type type) {
  cpr::Response response = cpr::Get(
      cpr::Url{url},
      cpr::Parameters{
          {"client_id", config.crawler_client_id},
          {"client_secret", config.crawler_client_secret},
      },
      cpr::Header{
          {"accept", "application/json"},
          {"Host", url_host},
          {"User-Agent", USERAGENT},
      });

  // rate_limit_limit     = std::stoi(response.header["X-RateLimit-Limit"]);
  // rate_limit_reset     = std::stoi(response.header["X-RateLimit-Reset"]);
  // rate_limit_remaining = std::stoi(response.header["X-RateLimit-Remaining"]);

  if (response.status_code == 403) {
    std::this_thread::sleep_for(std::chrono::seconds(30));
    spdlog::info("Wait for another 30s to request due to rate limit, X-RateLimit-Reset: {}", rate_limit_reset);
    return request(url, type);
  }

  if (response.status_code != 200) {
    spdlog::error("Got {} on request url: {}, {}", response.status_code, url, response.text);
    return REQUEST_ERROR;
  }

  if (response.text == "") {
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
    content = nlohmann::json::parse(response.text, cb);
  } catch (nlohmann::detail::parse_error &e) {
    spdlog::error("Request {} got error: {}", url.c_str(), e.what());
    return REQUEST_ERROR;
  }

  user user;
  int code = 0;
  switch (type) {
  case request_type_following:
  case request_type_followers:
    for (auto i : content) {
      code = request(url_prefix + "/users/" + i["login"].get<std::string>(), request_type_userinfo);
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
  std::string header = response.header["Link"];
  while (regex_search(header, result, pieces_regex)) {
    if (result.size() == 3 && result[2] == "next") {
      return request(result[1], type);
    }
    header = result.suffix().str();
  };

  return 0;
}

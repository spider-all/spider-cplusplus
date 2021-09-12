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

  int code = request(request_url, request_type_user);
  if (code != 0) {
    return code;
  }
  if (stopping) {
    return EXIT_SUCCESS;
  }

  semaphore++;
  std::thread followers_thread([=]() {
    spdlog::info("Followers thread is starting...");
    while (!stopping) {
      std::vector<std::string> users = database->list_users();
      for (const std::string &u : users) {
        std::string request_url = "/users/" + u + "/followers";
        int code = request(request_url, request_type_followers);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
        if (stopping) {
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("Followers thread stopped");
    semaphore--;
  });
  followers_thread.detach();

  semaphore++;
  std::thread followings_thread([=]() {
    spdlog::info("Following thread is starting...");
    while (!stopping) {
      std::vector<std::string> users = database->list_users();
      for (const std::string &u : users) {
        std::string request_url = "/users/" + u + "/following";
        int code = request(request_url, request_type_following);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
        if (stopping) {
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("Following thread stopped");
    semaphore--;
  });
  followings_thread.detach();

  semaphore++;
  std::thread orgs_thread([=]() {
    spdlog::info("Orgs thread is starting...");
    while (!stopping) {
      std::vector<std::string> users = database->list_users();
      for (const std::string &u : users) {
        std::string request_url = "/users/" + u + "/orgs";
        int code = request(request_url, request_type_orgs);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
        if (stopping) {
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("Orgs thread stopped");
    semaphore--;
  });
  orgs_thread.detach();

  semaphore++;
  std::thread orgs_member_thread([=]() {
    spdlog::info("Orgs thread is starting...");
    while (!stopping) {
      std::vector<std::string> orgs = database->list_orgs();
      for (const std::string &org : orgs) {
        std::string request_url = "/orgs/" + org + "/public_members";
        int code = request(request_url, request_type_orgs_member);
        if (code != 0) {
          spdlog::error("Request url: {} with error: {}", request_url, code);
        }
        if (stopping) {
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("Orgs member thread stopped");
    semaphore--;
  });
  orgs_member_thread.detach();

  semaphore++;
  std::thread repos_thread([=]() {
    spdlog::info("Repos thread is starting...");
    // while (!stopping) {
    //   int count = database->count_user();
    //   spdlog::info("Database have users: {}", count);
    //   std::this_thread::sleep_for(std::chrono::seconds(5));
    // }
    spdlog::info("Repos thread stopped");
    semaphore--;
  });
  repos_thread.detach();

  semaphore++;
  std::thread emojis_thread([=]() {
    spdlog::info("Emoji thread is starting...");
    std::string request_url = "/emojis";
    int code = request(request_url, request_type_emoji);
    if (code != 0) {
      spdlog::error("Request url: {} with error: {}", request_url, code);
    }
    spdlog::info("Emoji thread stopped");
    semaphore--;
  });
  emojis_thread.detach();

  semaphore++;
  std::thread gitignore_list_thread([=]() {
    spdlog::info("Gitignore list thread is starting...");
    std::string request_url = "/gitignore/templates";
    int code = request(request_url, request_type_gitignore_list);
    if (code != 0) {
      spdlog::error("Request url: {} with error: {}", request_url, code);
    }
    spdlog::info("Gitignore list thread stopped");
    semaphore--;
  });
  gitignore_list_thread.detach();

  semaphore++;
  std::thread license_list_thread([=]() {
    spdlog::info("License list thread is starting...");
    std::string request_url = "/licenses";
    int code = request(request_url, request_type_license_list);
    if (code != 0) {
      spdlog::error("Request url: {} with error: {}", request_url, code);
    }
    spdlog::info("License list thread stopped");
    semaphore--;
  });
  license_list_thread.detach();

  semaphore++;
  std::thread info_thread([=]() {
    spdlog::info("Info thread is starting...");
    while (!stopping) {
      int user_count = database->count_user();
      int org_count = database->count_org();
      spdlog::info("Database have users: {}, orgs: {}", user_count, org_count);

      fort::char_table table;
      table.set_border_style(FT_DOUBLE2_STYLE);
      table << fort::header
            << "Catalog"
            << "Count" << fort::endr
            << "users"
            << user_count << fort::endr
            << "orgs"
            << org_count << fort::endr;

      table.column(1).set_cell_text_align(fort::text_align::center);

      std::cout << std::endl
                << table.to_string() << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    spdlog::info("Info thread stopped");
    semaphore--;
  });
  info_thread.detach();

  return EXIT_SUCCESS;
}

int Request::request(const std::string &url, enum request_type type) {
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
      {"Authorization", "Bearer " + config.crawler_token},
  };
  auto response = client.Get(url.c_str(), headers);
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
    auto current = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    for (;;) {
      spdlog::info("Wait for another {}s to request due to rate limit, X-RateLimit-Reset: {}", rate_limit_reset - current, rate_limit_reset);
      std::this_thread::sleep_for(std::chrono::seconds(30));
      current = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      if (rate_limit_reset - current <= 0) {
        break;
      }
    }
    return request(url, type);
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
    code = request_followx(content);
    if (code != 0) {
      spdlog::error("Request userinfo with error: {}", code);
    }
    break;
  case request_type_orgs:
    code = request_orgs(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_orgs_member:
    code = request_orgs_members(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_user:
    code = request_user(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_emoji:
    code = request_emoji(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_gitignore_list:
    code = request_gitignore_list(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_gitignore_info:
    code = request_gitignore_info(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_license_list:
    code = request_license_list(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  case request_type_license_info:
    code = request_license_info(content);
    if (code != 0) {
      spdlog::error("Database with error: {}", code);
    }
    break;
  default:
    return UNKNOWN_REQUEST_TYPE;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(config.crawler_sleep_each_request));

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

      return request(u, type);
    }
    header_link = result.suffix().str();
  }

  return EXIT_SUCCESS;
}

int Request::request_followx(nlohmann::json content) {
  for (auto i : content) {
    int code = request("/users/" + i["login"].get<std::string>(), request_type_user);
    if (code != 0) {
      spdlog::error("Request userinfo with error: {}", code);
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs_members(nlohmann::json content) {
  for (auto con : content) {
    std::string request_url = "/users/" + con["login"].get<std::string>();
    int code = request(request_url, request_type_user);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs(nlohmann::json content) {
  for (auto con : content) {
    Org org;
    org.login = con["login"].get<std::string>();
    org.id = con["id"].get<int64_t>();
    org.node_id = con["node_id"].get<std::string>();
    org.description = con["description"].get<std::string>();
    int code = database->create_org(org);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_user(nlohmann::json content) {
  User user;
  if (content["hireable"].dump() != "true") {
    content["hireable"] = false;
  }
  user.login = content["login"].get<std::string>();
  user.id = content["id"].get<int64_t>();
  user.node_id = content["node_id"].get<std::string>();
  user.type = content["type"].get<std::string>();
  user.name = content["name"].get<std::string>();
  user.company = content["company"].get<std::string>();
  user.blog = content["blog"].get<std::string>();
  user.location = content["location"].get<std::string>();
  user.email = content["email"].get<std::string>();
  user.hireable = content["hireable"].get<bool>();
  user.bio = content["bio"].get<std::string>();
  user.created_at = content["created_at"].get<std::string>();
  user.updated_at = content["updated_at"].get<std::string>();
  user.public_gists = content["public_gists"].get<int>();
  user.public_repos = content["public_repos"].get<int>();
  user.following = content["following"].get<int>();
  user.followers = content["followers"].get<int>();

  int code = database->create_user(user);
  return code;
}

int Request::request_emoji(nlohmann::json content) {
  std::vector<Emoji> emojis;
  for (auto it : content.items()) {
    emojis.push_back(Emoji{it.key(), it.value()});
  }
  int code = database->create_emoji(emojis);
  return code;
}

int Request::request_gitignore_list(nlohmann::json content) {
  for (auto con : content) {
    std::string request_url = "/gitignore/templates/" + con.get<std::string>();
    int code = request(request_url, request_type_gitignore_info);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_gitignore_info(nlohmann::json content) {
  Gitignore gitignore = {
      content["name"].get<std::string>(),
      content["source"].get<std::string>(),
  };
  int code = database->create_gitignore(gitignore);
  return code;
}

int Request::request_license_list(nlohmann::json content) {
  for (auto con : content) {
    std::string request_url = "/licenses/" + con["key"].get<std::string>();
    int code = request(request_url, request_type_license_info);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_license_info(nlohmann::json content) {
  License license = {
      .key = content["key"].get<std::string>(),
      .name = content["name"].get<std::string>(),
      .spdx_id = content["spdx_id"].get<std::string>(),
      .node_id = content["node_id"].get<std::string>(),
      .description = content["description"].get<std::string>(),
      .implementation = content["implementation"].get<std::string>(),
      .permissions = content["permissions"].dump(),
      .conditions = content["conditions"].dump(),
      .limitations = content["limitations"].dump(),
      .body = content["body"].get<std::string>(),
      .featured = content["featured"].get<bool>(),
  };
  int code = database->create_license(license);
  return code;
}

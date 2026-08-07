#include <application/request.h>

#include <string_utils.h>

int Request::startup_orgs() {
  if (config.crawler_type_orgs) {
    semaphore++;
    std::thread orgs_thread([=, this]() {
      spdlog::info("orgs thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_users_random();
        for (const std::string &u : users) {
          std::vector<std::string> parts = string_split(u, KEYS_DELIMITER[0]);
          if (parts.size() != 2) {
            spdlog::error("invalid user record: {}", u);
            continue;
          }
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = "/users/" + parts[1] + "/orgs?per_page=100",
          };
          int code = request(request_config, request_type_orgs, request_type_orgs);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("orgs thread stopped");
      semaphore--;
    });
    orgs_thread.detach();
  }
  if (config.crawler_type_orgs_member) {
    semaphore++;
    std::thread orgs_member_thread([=, this]() {
      spdlog::info("orgs thread is starting...");
      while (!stopping) {
        std::vector<std::string> orgs = database->list_orgs_random();
        for (const std::string &org : orgs) {
          std::vector<std::string> parts = string_split(org, KEYS_DELIMITER[0]);
          if (parts.size() != 2) {
            spdlog::error("invalid org record: {}", org);
            continue;
          }
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = "/orgs/" + parts[1] + "/public_members?per_page=100",
          };
          int code = request(request_config, request_type_orgs_member, request_type_orgs_member);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("orgs member thread stopped");
      semaphore--;
    });
    orgs_member_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs_members(const nlohmann::json &content, enum request_type type_from) {
  for (auto con : content) {
    WRAP_FUNC(request_user_detail(con["login"].get<std::string>(), type_from))
    if (this->stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs(const nlohmann::json &content, enum request_type type_from) {
  std::vector<Org> orgs;
  for (auto con : content) {
    std::string login = con["login"].get<std::string>();
    bool updated = false;
    int64_t remaining_seconds = 0;
    WRAP_FUNC(this->database->update_version_if_recent("orgs", "login", login, DETAIL_REFRESH_SKIP_SECONDS, updated, remaining_seconds))
    if (updated) {
      spdlog::info("skip org refresh within 12h: {}, remaining: {}", login, format_duration(remaining_seconds));
      continue;
    }

    Org org{
        .id = con["id"].get<int64_t>(),
        .login = login,
        .node_id = con["node_id"].get<std::string>(),
        .description = con["description"].get<std::string>(),
        .followers = con.contains("followers") && !con["followers"].is_null()
                         ? con["followers"].get<int64_t>()
                         : 0,
    };
    orgs.push_back(org);
  }
  WRAP_FUNC(this->database->upsert_org_with_version(orgs, type_from))
  if (stopping) {
    return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
}

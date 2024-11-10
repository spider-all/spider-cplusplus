#include <application/request.h>

int Request::startup_orgs() {
  if (config.crawler_type_orgs) {
    semaphore++;
    std::thread orgs_thread([this]() {
      spdlog::info("Orgs thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_users_random(request_type_orgs);
        for (const std::string &u : users) {
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = "/users/" + u + "/orgs?per_page=100",
          };
          int code = request(request_config, request_type_orgs, request_type_orgs);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_config.path, code);
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
  }
  if (config.crawler_type_orgs_member) {
    semaphore++;
    std::thread orgs_member_thread([this]() {
      spdlog::info("Orgs thread is starting...");
      while (!stopping) {
        std::vector<std::string> orgs = database->list_orgs_random(request_type_orgs_repos);
        for (const std::string &org : orgs) {
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = "/orgs/" + org + "/public_members?per_page=100",
          };
          int code = request(request_config, request_type_orgs_member, request_type_orgs_member);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_config.path, code);
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
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs_members(const nlohmann::json &content, enum request_type type_from) {
  for (auto con : content) {
    RequestConfig request_config{
        .host = this->default_url_prefix,
        .path = "/users/" + con["login"].get<std::string>(),
    };
    WRAP_FUNC(request(request_config, request_type_user, type_from))
    if (this->stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs(const nlohmann::json &content, enum request_type type_from) {
  std::vector<Org> orgs;
  for (auto con : content) {
    Org org{
        .id = con["id"].get<int64_t>(),
        .login = con["login"].get<std::string>(),
        .node_id = con["node_id"].get<std::string>(),
        .description = con["description"].get<std::string>(),
    };
    orgs.push_back(org);
  }
  WRAP_FUNC(this->database->upsert_org_with_version(orgs, type_from))
  if (stopping) {
    return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
}

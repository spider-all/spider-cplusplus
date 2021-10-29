#include <application/request.h>

int Request::startup_orgs() {
  if (config.crawler_type_orgs) {
    semaphore++;
    std::thread orgs_thread([=]() {
      spdlog::info("Orgs thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_users_random(request_type_orgs);
        for (const std::string &u : users) {
          std::string request_url = "/users/" + u + "/orgs";
          int code = request(request_url, request_type_orgs, request_type_orgs);
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
  }
  if (config.crawler_type_orgs_member) {
    semaphore++;
    std::thread orgs_member_thread([=]() {
      spdlog::info("Orgs thread is starting...");
      while (!stopping) {
        std::vector<std::string> orgs = database->list_orgs_random(request_type_orgs_repos);
        for (const std::string &org : orgs) {
          std::string request_url = "/orgs/" + org + "/public_members";
          int code = request(request_url, request_type_orgs_member, request_type_orgs_member);
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
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs_members(const nlohmann::json &content, enum request_type type_from) {
  for (auto con : content) {
    std::string request_url = "/users/" + con["login"].get<std::string>();
    int code = request(request_url, request_type_user, type_from);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_orgs(const nlohmann::json &content, enum request_type type_from) {
  for (auto con : content) {
    Org org;
    org.login = con["login"].get<std::string>();
    org.id = con["id"].get<int64_t>();
    org.node_id = con["node_id"].get<std::string>();
    org.description = con["description"].get<std::string>();
    int code = database->upsert_org_with_version(org, type_from);
    if (code != 0) {
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

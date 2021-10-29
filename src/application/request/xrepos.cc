#include <application/request.h>

int Request::startup_xrepos() {
  if (config.crawler_type_users_repos) {
    semaphore++;
    std::thread users_repos_thread([=]() {
      spdlog::info("Users repos thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_users_random(request_type_users_repos);
        for (const std::string &u : users) {
          std::string request_url = "/users/" + u + "/repos?per_page=100";
          int code = request(request_url, request_type_users_repos, request_type_users_repos);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_url, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("Repos thread stopped");
      semaphore--;
    });
    users_repos_thread.detach();
  }

  if (config.crawler_type_orgs_repos) {
    semaphore++;
    std::thread orgs_repos_thread([=]() {
      spdlog::info("Repos thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_orgs_random(request_type_orgs_repos);
        for (const std::string &u : users) {
          std::string request_url = "/orgs/" + u + "/repos?per_page=100";
          int code = request(request_url, request_type_orgs_repos, request_type_orgs_repos);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_url, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("Repos thread stopped");
      semaphore--;
    });
    orgs_repos_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_repo_list(nlohmann::json content, enum request_type type_from) {
  int code = 0;
  for (auto &&con : content) {
    Repo repo{
        .id = con["id"].get<int64_t>(),
        .node_id = con["node_id"].get<std::string>(),
        .name = con["name"].get<std::string>(),
        .full_name = con["full_name"].get<std::string>(),
        .xprivate = con["private"].get<bool>(),
        .owner = con["owner"]["login"].get<std::string>(),
        .owner_type = con["owner"]["type"].get<std::string>(),
        .description = con["description"].get<std::string>(),
        .fork = con["fork"].get<bool>(),
        .created_at = con["created_at"].get<std::string>(),
        .updated_at = con["updated_at"].get<std::string>(),
        .pushed_at = con["pushed_at"].get<std::string>(),
        .homepage = con["homepage"].get<std::string>(),
        .size = con["size"].get<int64_t>(),
        .stargazers_count = con["stargazers_count"].get<int64_t>(),
        .watchers_count = con["watchers_count"].get<int64_t>(),
        .forks_count = con["forks_count"].get<int64_t>(),
        .language = con["language"].get<std::string>(),
        .forks = con["forks"].get<int64_t>(),
        .open_issues = con["open_issues"].get<int64_t>(),
        .watchers = con["watchers"].get<int64_t>(),
        .default_branch = con["default_branch"].get<std::string>(),
    };
    if (!con["license"].is_string()) {
      repo.license = con["license"]["key"].get<std::string>();
    }
    code = database->create_repo(repo, type_from);
    if (code != 0) {
      return code;
    }
  }
  return code;
}

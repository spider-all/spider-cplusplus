#include <application/request.h>

int Request::startup_repos_branches() {
  if (this->config.crawler_type_users_repos_branches) {
    this->semaphore++;
    std::thread users_repos_branches_thread([this]() {
      spdlog::info("Users repos branches thread is starting...");
      while (!stopping) {
        std::vector<std::string> repos = database->list_repos_random(request_type_users_repos);
        for (const std::string &repo : repos) {
          std::vector<std::string> repo_list;
          boost::algorithm::split(repo_list, repo, boost::algorithm::is_any_of(KEYS_DELIMITER));
          if (repo_list.size() != 2) {
            spdlog::error("Invalid repo: {}", repo);
            continue;
          }
          ExtraData extra;
          extra.repo = repo_list[0];
          extra.user = repo_list[1];
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = "/repos/" + repo_list[1] + "/" + repo_list[0] + "/branches?per_page=100",
          };
          request_config.extra = extra;
          int code = request(request_config, request_type_users_repos_branches, request_type_users_repos_branches);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("Users repos branches thread stopped");
      semaphore--;
    });
    users_repos_branches_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_repo_branches(nlohmann::json content, ExtraData extra, enum request_type type_from) {
  std::vector<Branch> branches;
  for (auto con : content) {
    Branch branch{
        .owner = extra.user,
        .repo = extra.repo,
        .name = con["name"],
        .commit = con["commit"]["sha"],
    };
    branches.push_back(branch);
  }
  WRAP_FUNC(database->upsert_branch(branches))
  return EXIT_SUCCESS;
}

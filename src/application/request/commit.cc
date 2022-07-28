#include <application/request.h>

int Request::startup_repos_branches_commits() {
  if (this->config.crawler_type_users_repos_branches_commits) {
    this->semaphore++;
    std::thread users_repos_branches_commits_thread([=, this]() {
      spdlog::info("Users repos branches commits thread is starting...");
      while (!stopping) {
        std::vector<std::string> branches = database->list_branches_random(request_type_users_repos);
        for (const std::string &branch : branches) {
          std::vector<std::string> branch_list;
          boost::algorithm::split(branch_list, branch, boost::algorithm::is_any_of(":"));
          if (branch_list.size() != 3) {
            spdlog::error("Invalid branch: {}", branch);
            continue;
          }
          ExtralData extral;
          extral.repo = branch_list[0];
          extral.user = branch_list[1];
          extral.branch = branch_list[2];
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = fmt::format("/repos/{}/{}/commits?sha={}&per_page=100", branch_list[1], branch_list[0], branch_list[2]),
          };
          request_config.extral = extral;
          int code = request(request_config, request_type_users_repos_branches_commits, request_type_users_repos_branches_commits);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("Users repos branches commits thread stopped");
      semaphore--;
    });
    users_repos_branches_commits_thread.detach();
  }
  return EXIT_SUCCESS;
}

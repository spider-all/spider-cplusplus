#include <application/request.h>

int Request::startup_repos_branches_commits() {
  if (this->config.crawler_type_users_repos_branches_commits) {
    this->semaphore++;
    std::thread users_repos_branches_commits_thread([=, this]() {
      spdlog::info("users repos branches commits thread is starting...");
      while (!stopping) {
        std::vector<std::string> branches = database->list_branches_random(request_type_users_repos);
        for (const std::string &branch : branches) {
          std::vector<std::string> branch_list;
          boost::algorithm::split(branch_list, branch, boost::algorithm::is_any_of(KEYS_DELIMITER));
          if (branch_list.size() != 4) {
            spdlog::error("invalid branch: {}", branch);
            continue;
          }
          ExtraData extra;
          extra.repo = branch_list[1];
          extra.user = branch_list[0];
          extra.branch = branch_list[2];
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = fmt::format("/repos/{}/{}/commits?sha={}&per_page=100", branch_list[0], branch_list[1], branch_list[2]),
          };
          request_config.extra = extra;
          int code = request(request_config, request_type_users_repos_branches_commits, request_type_users_repos_branches_commits);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("users repos branches commits thread stopped");
      semaphore--;
    });
    users_repos_branches_commits_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_commits(nlohmann::json content, ExtraData extra, enum request_type type_from) {
  if (!content.is_array()) {
    spdlog::error("unexpected commit response type: {}", content.type_name());
    return REQUEST_ERROR;
  }
  std::vector<Commit> commits;
  for (auto con : content) {
    if (!con.is_object()) {
      spdlog::error("unexpected commit element type: {}", con.type_name());
      continue;
    }
    Commit commit_data{
        .owner = extra.user,
        .repo = extra.repo,
        .branch = extra.branch,
        .sha = con.value("sha", ""),
        .node_id = con.value("node_id", ""),
        .commit = con["commit"].is_object() ? con["commit"].value("message", "") : "",
        .committer = con["committer"].is_object() ? con["committer"].value("login", "") : "",
        .author = con["author"].is_object() ? con["author"].value("login", "") : "",
        .message = con["commit"].is_object() ? con["commit"].value("message", "") : "",
        .url = con.value("url", ""),
        .comment_count = con["commit"].is_object() ? std::to_string(con["commit"].value("comment_count", 0)) : "0",
        .parents = "",
    };
    commits.push_back(commit_data);
  }
  WRAP_FUNC(database->upsert_commit(commits))
  return EXIT_SUCCESS;
}

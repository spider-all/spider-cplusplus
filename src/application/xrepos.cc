#include <application/request.h>

#include <chrono>

#include <string_utils.h>

namespace {
std::string ossinsight_language_query_value(const std::string &language) {
  switch (language.size()) {
  case 2:
    if (language == "C#") {
      return "C%23";
    }
    break;
  case 3:
    if (language == "C++") {
      return "C%2B%2B";
    }
    break;
  case 10:
    if (language == "Emacs Lisp") {
      return "Emacs%20Lisp";
    }
    break;
  case 11:
    if (language == "Common Lisp") {
      return "Common%20Lisp";
    }
    break;
  default:
    break;
  }
  return language;
}

Repo repo_from_json(const nlohmann::json &con) {
  Repo repo{
      .id = con["id"].get<int64_t>(),
      .node_id = con["node_id"].get<std::string>(),
      .name = con["name"].get<std::string>(),
      .full_name = con["full_name"].get<std::string>(),
      .xprivate = con["private"].get<bool>(),
      .owner = con["owner"]["login"].get<std::string>(),
      .owner_type = con["owner"]["type"].get<std::string>(),
      .description = con.value("description", ""),
      .fork = con["fork"].get<bool>(),
      .created_at = con["created_at"].get<std::string>(),
      .updated_at = con["updated_at"].get<std::string>(),
      .pushed_at = con["pushed_at"].get<std::string>(),
      .homepage = con.value("homepage", ""),
      .size = con["size"].get<int64_t>(),
      .stargazers_count = con["stargazers_count"].get<int64_t>(),
      .watchers_count = con["watchers_count"].get<int64_t>(),
      .forks_count = con["forks_count"].get<int64_t>(),
      .language = con.value("language", ""),
      .forks = con["forks"].get<int64_t>(),
      .open_issues = con["open_issues"].get<int64_t>(),
      .watchers = con["watchers"].get<int64_t>(),
      .default_branch = con.value("default_branch", ""),
  };
  if (!con["license"].is_null() && !con["license"].is_string()) {
    repo.license = con["license"].value("key", "");
  }
  return repo;
}
} // namespace

int Request::startup_xrepos() {
  auto request_config_repo = [this](std::string repo_name) -> int {
    string_trim(repo_name);
    if (repo_name.empty()) {
      return EXIT_SUCCESS;
    }

    std::vector<std::string> parts = string_split(repo_name, '/');
    if (parts.size() != 2 || parts[0].empty() || parts[1].empty()) {
      spdlog::error("invalid config repo name: {}", repo_name);
      return REQUEST_ERROR;
    }

    RequestConfig request_config{
        .host = this->default_url_prefix,
        .path = "/repos/" + repo_name,
    };
    int code = request(request_config, request_type_config_repos, request_type_config_repos);
    if (code != 0) {
      spdlog::error("request url: {} with error: {}", request_config.path, code);
    }
    return code;
  };

  if (!config.repository_names.empty()) {
    semaphore++;
    std::thread config_repos_thread([this, request_config_repo]() {
      spdlog::info("config repos thread is starting...");
      const auto interval = std::chrono::minutes(config.repository_refresh_interval_minutes);
      auto next_refresh = std::chrono::steady_clock::now();

      while (!stopping) {
        auto now = std::chrono::steady_clock::now();
        if (now < next_refresh) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          continue;
        }
        next_refresh = now + interval;

        for (std::string repo_name : config.repository_names) {
          request_config_repo(repo_name);
          if (stopping) {
            break;
          }
        }
      }

      spdlog::info("config repos thread stopped");
      semaphore--;
    });
    config_repos_thread.detach();
  }

  if (!config.repository_trend_languages.empty()) {
    semaphore++;
    std::thread trend_repos_thread([this]() {
      spdlog::info("trend repos thread is starting...");
      const auto interval = std::chrono::minutes(config.repository_refresh_interval_minutes);
      auto next_refresh = std::chrono::steady_clock::now();

      while (!stopping) {
        auto now = std::chrono::steady_clock::now();
        if (now < next_refresh) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          continue;
        }
        next_refresh = now + interval;

        for (std::string language : config.repository_trend_languages) {
          string_trim(language);
          if (language.empty()) {
            continue;
          }

          RequestConfig request_config{
              .host = "https://api.ossinsight.io",
              .path = "/v1/trends/repos/?period=past_week&language=" + ossinsight_language_query_value(language),
          };
          int code = request(request_config, request_type_trend_repos, request_type_trend_repos, true);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
      }

      spdlog::info("trend repos thread stopped");
      semaphore--;
    });
    trend_repos_thread.detach();
  }

  if (config.crawler_type_users_repos) {
    semaphore++;
    std::thread users_repos_thread([this]() {
      spdlog::info("users repos thread is starting...");
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
              .path = "/users/" + parts[1] + "/repos?per_page=100",
          };
          int code = request(request_config, request_type_users_repos, request_type_users_repos);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        // std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("repos thread stopped");
      semaphore--;
    });
    users_repos_thread.detach();
  }

  if (config.crawler_type_orgs_repos) {
    semaphore++;
    std::thread orgs_repos_thread([this]() {
      spdlog::info("repos thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_orgs_random();
        for (const std::string &u : users) {
          std::vector<std::string> parts = string_split(u, KEYS_DELIMITER[0]);
          if (parts.size() != 2) {
            spdlog::error("invalid org record: {}", u);
            continue;
          }
          RequestConfig request_config{
              .host = this->default_url_prefix,
              .path = "/orgs/" + parts[1] + "/repos?per_page=100",
          };
          int code = request(request_config, request_type_orgs_repos, request_type_orgs_repos);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("repos thread stopped");
      semaphore--;
    });
    orgs_repos_thread.detach();
  }
  if (config.crawler_type_starred) {
    semaphore++;
    std::thread starred_thread([this]() {
      spdlog::info("starred repos thread is starting...");
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
              .path = "/users/" + parts[1] + "/starred?per_page=100",
              .extra = {
                  .user_id = std::stoll(parts[0]),
              },
          };
          int code = request(request_config, request_type_starred, request_type_starred);
          if (code != 0) {
            spdlog::error("request url: {} with error: {}", request_config.path, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("starred repos thread stopped");
      semaphore--;
    });
    starred_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_repo_list(nlohmann::json content, enum request_type type_from) {
  if (!content.is_array()) {
    nlohmann::json arr = nlohmann::json::array();
    arr.push_back(content);
    content = arr;
  }
  std::vector<Repo> repos;
  for (auto &&con : content) {
    repos.push_back(repo_from_json(con));
  }
  return database->upsert_repo_with_version(repos, type_from);
}

int Request::request_trending_repos(const nlohmann::json &content) {
  if (!content.contains("data") || !content["data"].contains("rows") || !content["data"]["rows"].is_array()) {
    spdlog::error("invalid ossinsight trends response");
    return REQUEST_ERROR;
  }

  for (const auto &row : content["data"]["rows"]) {
    std::string repo_name = row.value("repo_name", "");
    string_trim(repo_name);
    if (repo_name.empty()) {
      continue;
    }

    RequestConfig request_config{
        .host = this->default_url_prefix,
        .path = "/repos/" + repo_name,
    };
    int code = request(request_config, request_type_config_repos, request_type_config_repos);
    if (code != 0) {
      spdlog::error("request url: {} with error: {}", request_config.path, code);
    }
    if (stopping) {
      break;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_starred(nlohmann::json content, const ExtraData &extra) {
  if (!content.is_array()) {
    nlohmann::json arr = nlohmann::json::array();
    arr.push_back(content);
    content = arr;
  }
  for (auto &&con : content) {
    Repo repo = repo_from_json(con);
    WRAP_FUNC(this->database->upsert_repo(repo))
    if (extra.user_id != 0) {
      WRAP_FUNC(this->database->upsert_starred(extra.user_id, repo.id))
    }
  }
  return EXIT_SUCCESS;
}

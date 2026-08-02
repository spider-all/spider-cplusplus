#include <application/request.h>

int Request::startup_events() {
  if (!config.crawler_type_events) {
    return EXIT_SUCCESS;
  }
  semaphore++;
  std::thread events_thread([=, this]() {
    spdlog::info("events thread is starting...");
    while (!stopping) {
      RequestConfig request_config{
          .host = this->default_url_prefix,
          .path = "/events?per_page=100",
      };
      int code = request(request_config, request_type_events, request_type_events);
      if (code != 0) {
        spdlog::error("request events with error: {}", code);
      }
      std::this_thread::sleep_for(std::chrono::seconds(60));
    }
    spdlog::info("events thread stopped");
    semaphore--;
  });
  events_thread.detach();
  return EXIT_SUCCESS;
}

int Request::request_events(const nlohmann::json &content) {
  for (auto &event : content) {
    if (stopping) {
      return EXIT_SUCCESS;
    }

    std::string event_type = event["type"].get<std::string>();

    // Extract actor (user) info
    if (event.contains("actor") && !event["actor"].is_null()) {
      std::string login = event["actor"]["login"].get<std::string>();
      if (!login.empty()) {
        RequestConfig user_config{
            .host = this->default_url_prefix,
            .path = "/users/" + login,
        };
        int code = request(user_config, request_type_user, request_type_events);
        if (code != 0) {
          spdlog::error("request user {} from events with error: {}", login, code);
        }
      }
    }

    // For PushEvent, also extract repo info
    if (event_type == "PushEvent" && event.contains("repo") && !event["repo"].is_null()) {
      std::string repo_name = event["repo"]["name"].get<std::string>();
      if (!repo_name.empty()) {
        RequestConfig repo_config{
            .host = this->default_url_prefix,
            .path = "/repos/" + repo_name,
        };
        int code = request(repo_config, request_type_users_repos, request_type_events);
        if (code != 0) {
          spdlog::error("request repo {} from events with error: {}", repo_name, code);
        }
      }
    }

    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

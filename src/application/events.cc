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
      for (int i = 0; i < 600 && !stopping; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
    spdlog::info("events thread stopped");
    semaphore--;
  });
  events_thread.detach();
  return EXIT_SUCCESS;
}

int Request::request_events(const nlohmann::json &content) {
  if (!content.is_array()) {
    spdlog::error("events response is not an array: type={}", content.type_name());
    return REQUEST_ERROR;
  }

  for (auto &event : content) {
    if (stopping) {
      return EXIT_SUCCESS;
    }

    std::string event_type = event.value("type", std::string{});
    std::string actor_login;
    std::string repo_name;

    if (event.contains("actor") && event["actor"].is_object()) {
      actor_login = event["actor"].value("login", std::string{});
    }
    if (event.contains("repo") && event["repo"].is_object()) {
      repo_name = event["repo"].value("name", std::string{});
    }

    if (actor_login == "Copilot") {
      spdlog::info("skip event from Copilot actor: type={}, repo={}", event_type, repo_name);
      continue;
    }

    // Extract actor (user) info
    if (!actor_login.empty()) {
      int code = request_user_detail(actor_login, request_type_events);
      if (code != 0) {
        spdlog::error("request user {} from events with error: {}", actor_login, code);
      }
    }

    // For PushEvent, also extract repo info
    if (event_type == "PushEvent" && !repo_name.empty()) {
      int code = request_repo_detail(repo_name, request_type_users_repos, request_type_events);
      if (code != 0) {
        spdlog::error("request repo {} from events with error: {}", repo_name, code);
      }
    }

    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

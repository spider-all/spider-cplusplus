#include <application/request.h>

int Request::startup_gitignore() {
  if (config.crawler_type_gitignore_list) {
    semaphore++;
    std::thread gitignore_list_thread([=]() {
      spdlog::info("Gitignore list thread is starting...");
      RequestConfig request_config{
          .host = this->default_url_prefix,
          .path = "/gitignore/templates",
      };
      int code = request(request_config, request_type_gitignore_list, request_type_gitignore_list);
      if (code != 0) {
        spdlog::error("Request url: {} with error: {}", request_config.path, code);
      }
      spdlog::info("Gitignore list thread stopped");
      semaphore--;
    });
    gitignore_list_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_gitignore_list(const nlohmann::json &content, enum request_type type_from) {
  for (const auto &con : content) {
    RequestConfig request_config{
        .host = this->default_url_prefix,
        .path = "/gitignore/templates/" + con.get<std::string>(),
    };
    WRAP_FUNC(request(request_config, request_type_gitignore_info, type_from))
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_gitignore_info(nlohmann::json content, enum request_type type_from) {
  Gitignore gitignore = {
      content["name"].get<std::string>(),
      content["source"].get<std::string>(),
  };
  WRAP_FUNC(database->upsert_gitignore(gitignore))
  return EXIT_SUCCESS;
}

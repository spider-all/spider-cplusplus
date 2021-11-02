#include <application/request.h>

int Request::startup_emojis() {
  if (this->config.crawler_type_emojis) {
    this->semaphore++;
    std::thread emojis_thread([=]() {
      spdlog::info("Emoji thread is starting...");
      RequestConfig request_config{
          .host = this->default_url_prefix,
          .path = "/emojis",
      };
      int code = request(request_config, request_type_emoji, request_type_emoji);
      if (code != 0) {
        spdlog::error("Request url: {} with error: {}", request_config.path, code);
      }
      spdlog::info("Emoji thread stopped");
      semaphore--;
    });
    emojis_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_emoji(nlohmann::json content, enum request_type type_from) {
  std::vector<Emoji> emojis;
  for (const auto &it : content.items()) {
    emojis.push_back(Emoji{it.key(), it.value()});
  }
  WRAP_FUNC(database->upsert_emoji(emojis))
  return EXIT_SUCCESS;
}

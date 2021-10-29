#include <application/request.h>

int Request::startup_emojis() {
  if (config.crawler_type_emojis) {
    semaphore++;
    std::thread emojis_thread([=]() {
      spdlog::info("Emoji thread is starting...");
      std::string request_url = "/emojis";
      int code = request(request_url, request_type_emoji, request_type_emoji);
      if (code != 0) {
        spdlog::error("Request url: {} with error: {}", request_url, code);
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
  int code = database->create_emoji(emojis);
  return code;
}

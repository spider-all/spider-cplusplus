#include <application/request.h>

int Request::startup_license() {
  if (config.crawler_type_license_list) {
    semaphore++;
    std::thread license_list_thread([=]() {
      spdlog::info("License list thread is starting...");
      RequestConfig request_config{
          .host = this->default_url_prefix,
          .path = "/licenses",
      };
      int code = request(request_config, request_type_license_list, request_type_license_list);
      if (code != 0) {
        spdlog::error("Request url: {} with error: {}", request_config.path, code);
      }
      spdlog::info("License list thread stopped");
      semaphore--;
    });
    license_list_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_license_list(const nlohmann::json &content, enum request_type type_from) {
  for (auto con : content) {
    RequestConfig request_config{
        .host = this->default_url_prefix,
        .path = "/licenses/" + con["key"].get<std::string>(),
    };
    WRAP_FUNC(request(request_config, request_type_license_info, type_from))
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

int Request::request_license_info(nlohmann::json content, enum request_type type_from) {
  License license = {
      .key = content["key"].get<std::string>(),
      .name = content["name"].get<std::string>(),
      .spdx_id = content["spdx_id"].get<std::string>(),
      .node_id = content["node_id"].get<std::string>(),
      .description = content["description"].get<std::string>(),
      .implementation = content["implementation"].get<std::string>(),
      .permissions = content["permissions"].dump(),
      .conditions = content["conditions"].dump(),
      .limitations = content["limitations"].dump(),
      .body = content["body"].get<std::string>(),
      .featured = content["featured"].get<bool>(),
  };
  WRAP_FUNC(database->upsert_license_with_version(license, type_from))
  return EXIT_SUCCESS;
}

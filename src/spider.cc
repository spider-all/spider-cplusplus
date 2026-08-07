#include <algorithm>
#include <cctype>
#include <csignal>
#include <iostream>
#include <thread>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <config.h>
#include <const.h>

#include <application/request.h>
#include <database/sqlite.h>

bool keep_running = true; // test keep running

void callback(int) {
  std::cout << std::endl; // output a new line after CTRL-C
  keep_running = false;
}

Database *switcher(const Config &config) {
  Database *ret = new SQLiteDatabase(config.database_sqlite_path);
  if (ret->code != 0) {
    spdlog::error("Open database with error: {}", ret->code);
  }
  return ret;
}

spdlog::level::level_enum parse_log_level(std::string level) {
  std::transform(level.begin(), level.end(), level.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });

  if (level == "trace") {
    return spdlog::level::trace;
  }
  if (level == "debug") {
    return spdlog::level::debug;
  }
  if (level == "info") {
    return spdlog::level::info;
  }
  if (level == "warn" || level == "warning") {
    return spdlog::level::warn;
  }
  if (level == "error") {
    return spdlog::level::err;
  }
  if (level == "critical") {
    return spdlog::level::critical;
  }
  if (level == "off") {
    return spdlog::level::off;
  }
  spdlog::warn("unknown log level: {}, fallback to debug", level);
  return spdlog::level::debug;
}

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

int main(int argc, char const *argv[]) {
  CLI::App app{"GitHub Spider"};
  app.set_version_flag("--version", STRINGIZE_VALUE_OF(SPIDER_VERSION));
  std::string config_path;
  app.add_option("-c,--config", config_path, "config path");
  CLI11_PARSE(app, argc, argv)

  spdlog::set_pattern("[%L][%H:%M:%S][thread %t] %v");

  const std::string default_config = "/etc/spider-cplusplus/config.yaml";
  if (config_path.empty()) {
    config_path = default_config;
  }

  if (!std::filesystem::exists(config_path)) {
    spdlog::error("no such a file: {}", config_path);
    return EXIT_FAILURE;
  }

  Config config;
  int code = config.initialize(config_path);
  if (code != 0) {
    spdlog::error("Parse config with error: {}", code);
    return EXIT_FAILURE;
  }
  spdlog::set_level(parse_log_level(config.log_level));

  Database *database = switcher(config);
  if (database == nullptr) {
    return EXIT_FAILURE;
  }

  code = database->initialize();
  if (code != 0) {
    spdlog::error("Initialize database with error: {}", code);
    return EXIT_FAILURE;
  }

  Application *request = new Request(config, database);

  signal(SIGINT, callback);
  signal(SIGTERM, callback);

  code = request->startup();
  if (code != 0) {
    spdlog::error("Spider startup got error: {}", code);
    keep_running = false;
  }

  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
  }

  delete request;
  delete database;

  spdlog::info("All of applications stopped...");

  return EXIT_SUCCESS;
}

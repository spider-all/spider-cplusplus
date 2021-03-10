#include <csignal>
#include <iostream>
#include <thread>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <config.h>

#include <application/request.h>
#include <application/server.h>
#include <database/dynamo.h>
#include <database/level.h>
#include <database/redis.h>
#include <database/sqlite.h>

#define SPIDER_VERSION_MAJOR 1
#define SPIDER_VERSION_MINOR 2
#define SPIDER_VERSION_PATCH 0

bool keep_running = true; // test keep running

void callback(int) {
  std::cout << std::endl; // output a new line after CTRL-C
  keep_running = false;
}

Database *switcher(const Config &config) {
  Database *ret = nullptr;
  if (config.database_type == "sqlite3") {
    ret = new DBSQLite(config.database_path);
  } else if (config.database_type == "redis") {
    ret = new DBRedis(config.database_host, config.database_port);
  } else if (config.database_type == "leveldb") {
    ret = new DBLevel(config.database_path);
  } else if (config.database_type == "dynamo") {
    ret = new DynamoDB(config.database_aws_region);
  }
  if (ret == nullptr) {
    return nullptr;
  }
  if (ret->code != 0) {
    spdlog::error("Open database with error: {}", ret->code);
  }
  return ret;
}

int main(int argc, char const *argv[]) {
  std::ostringstream version;
  version << SPIDER_VERSION_MAJOR << "." << SPIDER_VERSION_MINOR << "." << SPIDER_VERSION_PATCH;

  CLI::App app{"GitHub Spider"};
  app.set_version_flag("--version", version.str());
  std::string config_path;
  app.add_option("-c,--config", config_path, "config path");
  CLI11_PARSE(app, argc, argv);

  const std::string default_config = "./etc/config.yaml";
  if (config_path.empty()) {
    config_path = default_config;
  }

  if (!std::filesystem::exists(config_path)) {
    spdlog::error("no such a file: {}", config_path);
    return EXIT_FAILURE;
  }

  Config config;
  int code = config.initialize(default_config.c_str());
  if (code != 0) {
    spdlog::error("Parse config with error: {}", code);
    return EXIT_FAILURE;
  }

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

  code = request->startup();
  if (code != 0) {
    spdlog::error("Spider startup got error: {}", code);
    keep_running = false;
  }

  Application *server = new Server(config, database);

  code = server->startup();
  if (code != 0) {
    spdlog::error("Server startup got error: {}", code);
    keep_running = false;
  }

  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
  }

  delete request;
  delete server;
  delete database;

  spdlog::info("All of applications stopped...");

  return EXIT_SUCCESS;
}

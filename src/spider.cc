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
#include <database/mongo.h>
#include <database/postgres.h>
#include <database/sqlite3.h>

bool keep_running = true; // test keep running

void callback(int) {
  std::cout << std::endl; // output a new line after CTRL-C
  keep_running = false;
}

Database *switcher(const Config &config) {
  Database *ret = nullptr;
  if (config.database_type == "sqlite3") {
    ret = new SQLite3(config.database_path);
  } else if (config.database_type == "leveldb") {
    ret = new Level(config.database_leveldb_path);
  } else if (config.database_type == "dynamo") {
    ret = new Dynamo(config.database_aws_region);
  } else if (config.database_type == "mongodb") {
    ret = new Mongo(config.database_host);
  } else if (config.database_type == "postgresql") {
    ret = new Postgres(config.database_host);
  }
  if (ret == nullptr) {
    return nullptr;
  }
  if (ret->code != 0) {
    spdlog::error("Open database with error: {}", ret->code);
  }
  return ret;
}

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

int main(int argc, char const *argv[]) {
  CLI::App app{"GitHub Spider"};
  app.set_version_flag("--version", STRINGIZE_VALUE_OF(SPIDER_VERSION));
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

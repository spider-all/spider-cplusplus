#include <csignal>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <spdlog/spdlog.h>

#include <application/request.h>
#include <application/server.h>
#include <cli.h>
#include <common.h>
#include <config.h>
#include <database/dbrd.h>
#include <database/dbrk.h>
#include <database/dbsq.h>

bool keep_running = true; // test keep running

void callback(int) {
  std::cout << std::endl; // output a new line after CTRL+C
  keep_running = false;
}

Database *switcher(const Config& config) {
  Database *ret = nullptr;
  if (config.database_type == "sqlite3") {
    ret = new DBSQ(config.database_path);
  } else if (config.database_type == "rocksdb") {
    ret = new DBRK(config.database_path);
  } else if (config.database_type == "redis") {
    ret = new DBRD(config.database_host, config.database_port);
  }
  if (ret == nullptr) {
    return nullptr;
  }
  if (ret->code != 0) {
    spdlog::error("Open database with error: {}", ret->code);
  }
  return ret;
}

int main(int argc, char *argv[]) {
  char *config_path = CommandLine::cli(argc, argv);
  if (config_path == nullptr) {
    return EXIT_SUCCESS;
  }

  Config config;
  int code = config.config(config_path);
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

  return EXIT_SUCCESS;
}

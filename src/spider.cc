#include <csignal>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <spdlog/spdlog.h>

#include <cli.h>
#include <common.h>
#include <config.h>
#include <dbrk.h>
#include <dbsq.h>
#include <request.h>
#include <server.h>

bool keep_running = true; // test keep running

void callback(int) {
  std::cout << std::endl; // output a new line after CTRL+C
  keep_running = false;
}

Database *switcher(std::string type, std::string path) {
  Database *ret = nullptr;
  if (type == "sqlite3") {
    ret = new DBSQ(path);
  } else if (type == "rocksdb") {
    ret = new DBRK(path);
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
  CommandLine commandLine;
  char *config_path = commandLine.cli(argc, argv);
  if (config_path == nullptr) {
    return EXIT_SUCCESS;
  }

  Config config;
  int code = config.config(config_path);
  if (code != 0) {
    spdlog::error("Parse config with error: {}", code);
    return EXIT_FAILURE;
  }

  Database *database = switcher(config.database_type, config.database_path);
  if (database == nullptr) {
    return EXIT_FAILURE;
  }

  code = database->initialize();
  if (code != 0) {
    spdlog::error("Initialize database with error: {}", code);
    return EXIT_FAILURE;
  };

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

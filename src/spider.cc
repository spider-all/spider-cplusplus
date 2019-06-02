#include <csignal>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <spdlog/spdlog.h>

#include <cli.hpp>
#include <config.hpp>
#include <request.hpp>
#include <server.hpp>

bool keep_running = true; // test keep running
httplib::Server svr;

void callback(int) {
  std::cout << std::endl; // output a new after CTRL+C
  if (svr.is_running()) {
    svr.stop();
  }
  keep_running = false;
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

  Database database;
  code = database.initialize(config.database_path);
  if (code != 0) {
    spdlog::error("Initialize database with error: {}", code);
    return EXIT_FAILURE;
  };

  Request request(config, database);

  signal(SIGINT, callback);

  code = request.startup();
  if (code != 0) {
    spdlog::error("Spider startup got error: {}", code);
    keep_running = false;
  };

  Server server(database);
  code = server.startup();
  if (code != 0) {
    spdlog::error("Server startup got error: {}", code);
    keep_running = false;
  }

  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
  }

  request.teardown();
  server.teardown();
  database.deinit();

  return EXIT_SUCCESS;
}

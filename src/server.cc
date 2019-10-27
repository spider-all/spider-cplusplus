#include <iostream>

#include <httplib.h>

#include <application.h>
#include <config.h>
#include <database.hpp>

#include <server.h>

#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT

Server::Server(Database db) {
  database = db;
}

int Server::startup() {
  std::thread server_thread([=]() {
    svr.Get("/hi", [](const httplib::Request & /*req*/, httplib::Response &res) {
      res.set_content("Hello World!", "text/plain");
    });
    spdlog::info("Server is running...");
    svr.listen("0.0.0.0", 1234);
  });
  server_thread.detach();
  return EXIT_SUCCESS;
}

void Server::teardown() {
  if (svr.is_running()) {
    svr.stop();
  }
  spdlog::info("Server running over...");
}

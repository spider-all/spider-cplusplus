#include <iostream>

#include <httplib.h>

#include <application.h>
#include <config.hpp>
#include <database.hpp>

#pragma once

#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT

class Server : Application {
private:
  Config config;
  Database database;
  httplib::Server svr;

public:
  Server(Database db) {
    database = db;
  }
  int startup() {
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
  void teardown() {
    if (svr.is_running()) {
      svr.stop();
    }
    spdlog::info("Server running over...");
  }
};

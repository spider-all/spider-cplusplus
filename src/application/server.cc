#include <application/server.h>

Server::Server(Config c, Database *db) {
  config = std::move(c);
  database = db;
}

Server::~Server() {
  stopping = true;

  if (svr.is_running()) {
    svr.stop();
  }

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
    if (semaphore == 0) {
      break;
    }
  }

  spdlog::info("Server stopped...");
}

int Server::startup() {
  semaphore++;
  std::thread server_thread([=]() {
    svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
      res.set_content("Hello World!", "text/plain");
    });
    int port = 3000;
    spdlog::info("Server running at port {}", port);
    svr.listen("0.0.0.0", port);
    semaphore--;
  });
  server_thread.detach();

  return EXIT_SUCCESS;
}

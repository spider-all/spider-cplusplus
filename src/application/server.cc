#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <boost/beast/version.hpp>

#include <application/server.h>

Server::Server(Config c, Database *db) {
  config   = c;
  database = db;
}

Server::~Server() {
  spdlog::info("Server running over...");
}

int Server::startup() {
  std::thread server_thread([=]() {
    boost::asio::io_service service;
    boost::asio::ip::tcp::acceptor acceptor(service);
    boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), 0));

    acceptor.open(endPoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endPoint);
    acceptor.listen(); // NEEDED TO ADD THIS BIT!
    unsigned short port = acceptor.local_endpoint().port();
    spdlog::info("Server running at port {}", port);
  });
  server_thread.detach();

  return EXIT_SUCCESS;
}

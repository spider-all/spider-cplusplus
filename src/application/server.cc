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
  // if (svr.is_up()) {
  //   svr.stop();
  // }
  spdlog::info("Server running over...");
}

// void get_method_handler(const std::shared_ptr<restbed::Session> session) {
//   session->close(restbed::OK, "Hello, World!", {{"Content-Length", "13"}});
// }

// void http_server(boost::asio::ip::tcp::acceptor &acceptor, boost::asio::ip::tcp::socket &socket) {
//   acceptor.async_accept(socket,
//                         [&](boost::beast::error_code ec) {
//                           if (!ec)
//                             std::make_shared<boost::asio::http_connection>(std::move(socket))->start();
//                           http_server(acceptor, socket);
//                         });
// }

int Server::startup() {
  std::thread server_thread([=]() {
    // svr.Get("/hi", [](const httplib::Request & /*req*/, httplib::Response &res) {
    //   res.set_content("Hello World!", "text/plain");
    // });
    // spdlog::info("Server is running...");
    // svr.listen("0.0.0.0", 1234);
    // auto resource = std::make_shared<restbed::Resource>();
    // resource->set_path("/resource");
    // resource->set_method_handler("GET", get_method_handler);

    // auto settings = std::make_shared<restbed::Settings>();
    // settings->set_port(1984);
    // settings->set_bind_address("127.0.0.1");
    // settings->set_default_header("Connection", "close");

    // svr.publish(resource);
    // svr.start(settings);
    // boost::asio::io_context ioc{1};

    // auto const address  = boost::asio::ip::;
    // unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));
    // ;
    boost::asio::io_service service;
    boost::asio::ip::tcp::acceptor acceptor(service);
    boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), 0));
    // boost::asio::ip::tcp::acceptor acceptor{ioc, {boost::asio::ip::address_v4::any(), (unsigned short)(0)}};
    // unsigned short port = acceptor.local_endpoint().port();
    // boost::asio::ip::tcp::socket socket{ioc};

    acceptor.open(endPoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endPoint);
    acceptor.listen(); // NEEDED TO ADD THIS BIT!
    unsigned short port = acceptor.local_endpoint().port();
    std::cout << port << std::endl;
    // ioc.run();
  });
  server_thread.detach();

  return EXIT_SUCCESS;
}

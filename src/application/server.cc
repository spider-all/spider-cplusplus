// #include <application/server.h>

// Server::Server(Config c, Database *db) {
//   config = std::move(c);
//   database = db;
// }

// Server::~Server() {
//   stopping = true;

//   if (svr.is_running()) {
//     svr.stop();
//   }

//   while (true) {
//     std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
//     if (semaphore == 0) {
//       break;
//     }
//   }

//   spdlog::info("Server stopped...");
// }

// common_args Server::helper(const httplib::Request &req) {
//   common_args args{0, 0, ""};
//   if (req.has_param("page")) {
//     args.page = std::stoll(req.get_param_value("page"));
//     if (args.page == 0) {
//       args.page = 1;
//     }
//   }
//   if (req.has_param("limit")) {
//     args.limit = std::stoll(req.get_param_value("limit"));
//     if (args.limit == 0 || args.limit > 100) {
//       args.limit = 100;
//     }
//   }
//   if (req.has_param("query")) {
//     args.query = req.get_param_value("query");
//   }
//   return args;
// }

// void to_json(nlohmann::json &j, const User &p) {
//   j = {{"login", p.login}};
// }

// int Server::startup() {
//   semaphore++;
//   std::thread server_thread([=, this]() {
//     svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
//       res.set_content("Hello World!", "text/plain");
//     });
//     svr.Get("/users", [=, this](const httplib::Request &req, httplib::Response &res) {
//       common_args args = helper(req);
//       std::vector<User> users = this->database->list_usersx(args);
//       nlohmann::json content = users;
//       res.set_content(content.dump(), "application/json");
//     });
//     int port = 3000;
//     spdlog::info("Server running at port {}", port);
//     svr.listen("0.0.0.0", port);
//     semaphore--;
//   });
//   server_thread.detach();

//   return EXIT_SUCCESS;
// }

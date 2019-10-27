#include <httplib.h>

#pragma once

#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT

class Server : public Application {
private:
  Config config;
  Database database;
  httplib::Server svr;

public:
  Server(Database db);
  int startup();
  void teardown();
};

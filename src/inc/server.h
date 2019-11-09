#include <iostream>

#pragma once

#define CPPHTTPLIB_ZLIB_SUPPORT

#include <httplib.h>
#include <sqlite.hpp>

#include <application.h>
#include <config.h>

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

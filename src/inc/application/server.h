#include <iostream>

#define CPPHTTPLIB_ZLIB_SUPPORT

#include <httplib.h>

#pragma once

#include <application/application.h>
#include <config.h>
#include <database/database.h>

class Server : public Application {
private:
  Config config;
  Database *database;
  httplib::Server svr;

public:
  Server(Config, Database *);
  ~Server();
  int startup();
};

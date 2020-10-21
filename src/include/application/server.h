#include <iostream>

#include <httplib.h>

#include <config.h>

#include <application/application.h>
#include <database/database.h>

#pragma once

class Server : public Application {
private:
  Config config;
  Database *database;

  int semaphore = 0; // 执行过程中的信号量
  bool stopping = false;

  httplib::Server svr;

public:
  Server(Config, Database *);
  ~Server() override;
  int startup() override;
};

#include <iostream>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <config.h>
#include <model.h>

#include <application.h>
#include <database.h>

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

  static common_args helper(const httplib::Request &req);
};

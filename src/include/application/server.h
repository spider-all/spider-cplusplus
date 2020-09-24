#include <iostream>

#pragma once

#include <application/application.h>
#include <config.h>
#include <database/database.h>

class Server : public Application {
private:
  Config config;
  Database *database;

public:
  Server(Config, Database *);
  ~Server() override;
  int startup() override;
};

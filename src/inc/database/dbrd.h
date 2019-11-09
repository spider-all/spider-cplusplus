#include <iostream>
#include <random>
#include <thread>

#include <hiredis/hiredis.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class DBRD : public Database {
private:
  int set_value(std::string, std::string);
  int get_value(std::string, std::string *);
  std::mutex mtx;

public:
  DBRD(const std::string&, int);
  ~DBRD();
  int initialize();
  int create_user(user);
  std::vector<std::string> list_users();
};

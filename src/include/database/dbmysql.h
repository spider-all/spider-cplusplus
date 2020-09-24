#include <iostream>
#include <random>
#include <thread>

// #include <Poco/Data/MySQL/Connector.h>
// #include <Poco/Data/Session.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class DBMySQL : public Database {
private:
  //  int set_value(std::string, std::string);
  //  int get_value(std::string, std::string *);
  //  std::mutex mtx;

public:
  DBMySQL(const std::string &, int);
  ~DBMySQL() override;
  int initialize() override;
  int create_user(user) override;
  std::vector<std::string> list_users() override;
};

#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>
#include <error.hpp>
#include <model.h>

class DBSQLite : public Database {
public:
  explicit DBSQLite(const std::string &);
  ~DBSQLite() override;
  int initialize() override;
  int create_user(user) override;
  std::vector<std::string> list_users() override;
  int64_t count_user() override;
};

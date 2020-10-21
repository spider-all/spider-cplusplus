#include <iostream>
#include <random>

#include <leveldb/db.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class DBLevel : public Database {
private:
  leveldb::Status status;

public:
  explicit DBLevel(const std::string &);
  ~DBLevel() override;
  int initialize() override;
  int create_user(user) override;
  std::vector<std::string> list_users() override;
  int count_user() override;
};

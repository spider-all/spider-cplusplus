#include <iostream>
#include <random>

#include <leveldb/db.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class Level : public Database {
private:
  leveldb::Status status;

public:
  explicit Level(const std::string &);
  ~Level() override;
  int initialize() override;
  int create_user(user) override;
  std::vector<std::string> list_users() override;
  int64_t count_user() override;
  int create_org(Org) override;
};

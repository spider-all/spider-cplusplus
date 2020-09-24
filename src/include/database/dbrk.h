#include <iostream>
#include <random>

#include <rocksdb/db.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class DBRK : public Database {
private:
  rocksdb::Status status;

public:
  explicit DBRK(const std::string&);
  ~DBRK() override;
  int initialize() override;
  int create_user(user) override;
  std::vector<std::string> list_users() override;
};

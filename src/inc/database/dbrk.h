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
  DBRK(std::string);
  ~DBRK();
  int initialize();
  int create_user(user);
  std::vector<std::string> list_users();
};

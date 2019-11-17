#include <iostream>

#include <rocksdb/db.h>

#pragma once

#include <database.h>

class DBRK : public Database {
public:
  DBRK(std::string);
  ~DBRK();
  int initialize();
  int create_user(user);
  std::vector<std::string> list_users();
};

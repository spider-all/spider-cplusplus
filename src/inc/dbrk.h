#include <iostream>

#include <rocksdb/db.h>

#pragma once

#include <database.h>

class DBRK : public Database {
public:
  DBRK(std::string);
  int initialize();
  void deinit();
  int create_user(user);
};

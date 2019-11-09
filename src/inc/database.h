#include <iostream>

#include <sqlite3.h>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    sqlite3 *sqlite;
  } db;
  int code;
  virtual int initialize()      = 0;
  virtual void deinit()         = 0;
  virtual int create_user(user) = 0;
};

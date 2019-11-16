#include <iostream>

#include <SQLiteCpp/SQLiteCpp.h>
#include <rocksdb/db.h>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    rocksdb::DB *rocksdb;
    SQLite::Database *sqlite;
  } db;
  int code = 0;

  virtual int initialize()                      = 0;
  virtual void deinit()                         = 0;
  virtual int create_user(user)                 = 0;
  virtual std::vector<std::string> list_users() = 0;
};

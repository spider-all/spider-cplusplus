#include <iostream>

#include <rocksdb/db.h>
#include <sqlite3.h>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    sqlite3 *sqlite;
    rocksdb::DB *rocksdb;
  } db;
  int code = 0;

  virtual int initialize()      = 0;
  virtual void deinit()         = 0;
  virtual int create_user(user) = 0;
};

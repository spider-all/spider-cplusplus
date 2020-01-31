#include <iostream>

//#include <mongocxx/client.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <rocksdb/db.h>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    rocksdb::DB *rocksdb;
    SQLite::Database *sqlite;
    // mongocxx::client mongo;
  } db;
  int code = 0;
  virtual ~Database(){};
  virtual int initialize()                      = 0;
  virtual int create_user(user)                 = 0;
  virtual std::vector<std::string> list_users() = 0;
};

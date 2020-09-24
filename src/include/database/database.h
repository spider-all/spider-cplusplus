#include <iostream>

// #include <Poco/Data/Session.h>
// #include <SQLiteCpp/SQLiteCpp.h>
// #include <hiredis/hiredis.h>
// #include <rocksdb/db.h>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    // rocksdb::DB *rocksdb;
    // SQLite::Database *sqlite;
    // redisContext *redis;
    // Poco::Data::Session *mysql;
  } db{};
  int code            = 0;
  virtual ~Database() = default;
  ;
  virtual int initialize()                      = 0;
  virtual int create_user(user)                 = 0;
  virtual std::vector<std::string> list_users() = 0;
};

#include <iostream>

#include <SQLiteCpp/SQLiteCpp.h>
#include <hiredis/hiredis.h>
#include <leveldb/db.h>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    leveldb::DB *rocksdb;
    SQLite::Database *sqlite;
    redisContext *redis;
  } db{};
  int code = 0;
  virtual ~Database() = default;
  virtual int initialize() = 0;
  virtual int create_user(user) = 0;
  virtual std::vector<std::string> list_users() = 0;
  virtual int count_user() = 0;
};

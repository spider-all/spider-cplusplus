#include <iostream>

#include <SQLiteCpp/SQLiteCpp.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <hiredis/hiredis.h>
#include <leveldb/db.h>
#include <mongocxx/client.hpp>
#include <pqxx/pqxx>

#pragma once

#include <model.h>

class Database {
public:
  struct db {
    leveldb::DB *leveldb;
    SQLite::Database *sqlite;
    redisContext *redis;
    Aws::DynamoDB::DynamoDBClient *dynamo;
    mongocxx::client *client;
    pqxx::connection *pg;
  } db{};
  int code = 0;
  virtual ~Database() = default;
  virtual int initialize() = 0;

  virtual int create_user(User) = 0;
  virtual int64_t count_user() = 0;
  virtual std::vector<std::string> list_users() = 0;

  virtual int create_org(Org) = 0;
  virtual int64_t count_org() = 0;
  virtual std::vector<std::string> list_orgs() = 0;

  virtual int create_emoji(std::vector<Emoji> emojis) = 0;
  virtual int64_t count_emoji() = 0;

  virtual int create_gitignore(Gitignore gitignore) = 0;
  virtual int64_t count_gitignore() = 0;

  virtual int create_license(License license) = 0;
  virtual int64_t count_license() = 0;
};

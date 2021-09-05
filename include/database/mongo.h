#include <iostream>
#include <random>
#include <thread>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <spdlog/spdlog.h>

#include <error.hpp>

#pragma once

#include <database/database.h>

class Mongo : public Database {
private:
  mongocxx::collection coll;

public:
  Mongo(const std::string &);
  ~Mongo() override;
  int initialize() override;

  int create_user(User) override;
  int64_t count_user() override;
  std::vector<std::string> list_users() override;

  int create_org(Org) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs() override;

  int create_emoji(std::vector<Emoji> emojis) override;
  int64_t count_emoji() override;
};

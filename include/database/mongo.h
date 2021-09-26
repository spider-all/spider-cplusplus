#include <iostream>
#include <random>
#include <thread>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <fmt/core.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <spdlog/spdlog.h>

#include <error.h>

#pragma once

#include <database/database.h>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class Mongo : public Database {
private:
  std::string dsn;
  mongocxx::uri *uri{};
  mongocxx::pool *pool{};

  int64_t count_x(const std::string &c);

  int64_t followers_version = 0;
  int64_t following_version = 0;

public:
  explicit Mongo(const std::string &);
  ~Mongo() override;
  int initialize() override;
  int initialize_version() override;

  int create_user(User user, enum request_type type) override;
  int64_t count_user() override;
  std::vector<std::string> list_users_random(std::string type) override;
  std::vector<User> list_usersx(common_args args) override;

  int create_org(Org) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs() override;

  int create_emoji(std::vector<Emoji> emojis) override;
  int64_t count_emoji() override;

  int create_gitignore(Gitignore gitignore) override;
  int64_t count_gitignore() override;

  int create_license(License license) override;
  int64_t count_license() override;

  int create_repo(Repo repo) override;
  int64_t count_repo() override;
};

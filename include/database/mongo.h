#include <iostream>
#include <random>
#include <thread>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
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
using bsoncxx::builder::basic::make_array;

class Mongo : public Database {
private:
  std::string dsn;
  mongocxx::uri *uri{};
  mongocxx::pool *pool{};

  const int32_t sample_size = 100;

  int64_t count_x(const std::string &c);

  static std::string function_name_helper(std::string func_name);

  int64_t followers_version = 1;
  int64_t following_version = 1;
  int64_t orgs_version = 1;
  int64_t orgs_member_version = 1;
  int64_t users_repos_version = 1;
  int64_t orgs_repos_version = 1;
  int64_t gitignore_list_version = 1;
  int64_t license_list_version = 1;

public:
  explicit Mongo(const std::string &);
  ~Mongo() override;
  int initialize() override;
  int initialize_version() override;

  int create_user(User user, enum request_type type) override;
  int update_user_version(User user, enum request_type type) override;
  int upsert_user(User user) override;
  int64_t count_user() override;
  std::vector<std::string> list_users_random(enum request_type type) override;
  std::vector<User> list_usersx(common_args args) override;

  int create_org(Org org, enum request_type type) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs_random(enum request_type type) override;

  int create_emoji(std::vector<Emoji> emojis) override;
  int64_t count_emoji() override;

  int create_gitignore(Gitignore gitignore) override;
  int64_t count_gitignore() override;

  int create_license(License license, enum request_type type) override;
  int64_t count_license() override;

  int create_repo(Repo repo, enum request_type type) override;
  int64_t count_repo() override;
};

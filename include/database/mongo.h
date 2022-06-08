#include <iostream>
#include <map>
#include <random>
#include <thread>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <fmt/core.h>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <spdlog/spdlog.h>

#include <common.h>
#include <error.h>
#include <versions.h>

#pragma once

#include <database.h>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

#define GET_CONNECTION_RAW(database_name) \
  auto connect = this->pool->acquire();   \
  auto database = connect->database(database_name);

#define GET_CONNECTION(database_name, collection_name) \
  GET_CONNECTION_RAW(database_name)                    \
  auto coll = database[collection_name];

class Mongo : public Database {
private:
  std::string dsn;
  mongocxx::uri *uri{};
  mongocxx::pool *pool{};

  Versions *versions;

  const int32_t sample_size = 100;

public:
  explicit Mongo(const std::string &);
  ~Mongo() override;
  int initialize() override;
  int initialize_version() override;

  int64_t count_x(const std::string &c);
  int insert_x(const std::string &collection, bsoncxx::document::view_or_value doc);
  int upsert_x(const std::string &collection, std::string filter, std::string update);
  int upsert_x(const std::string &collection, const std::map<std::string, std::string> &filters);
  std::vector<std::string> list_x_random(const std::string &collection, std::string key, enum request_type type);
  int ensure_index(const std::string &collection, std::vector<std::string> index);
  int create_x_collection(const std::string &collection, std::string key);

  int create_collections();

  int update_version(std::string key, enum request_type type) override;
  int update_version(std::vector<std::string> key, enum request_type type) override;
  int incr_version(enum request_type type) override;

  int upsert_user(User user) override;
  int upsert_user_with_version(User user, enum request_type type) override;
  int64_t count_user() override;
  std::vector<std::string> list_users_random(enum request_type type) override;
  std::vector<User> list_usersx(common_args args) override;

  int upsert_org(Org org) override;
  int upsert_org(std::vector<Org> orgs) override;
  int upsert_org_with_version(Org org, enum request_type type) override;
  int upsert_org_with_version(std::vector<Org> orgs, enum request_type type) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs_random(enum request_type type) override;

  int upsert_emoji(std::vector<Emoji> emojis) override;
  int64_t count_emoji() override;

  int upsert_gitignore(Gitignore gitignore) override;
  int64_t count_gitignore() override;

  int upsert_license(License license) override;
  int upsert_license_with_version(License license, enum request_type type) override;
  int64_t count_license() override;

  bsoncxx::document::value make_repo(Repo repo);
  int upsert_repo(Repo repo) override;
  int upsert_repo(std::vector<Repo> repos) override;
  int upsert_repo_with_version(Repo repo, enum request_type type) override;
  int upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) override;
  std::vector<std::string> list_repos_random(enum request_type type) override;
  int64_t count_repo() override;

  bsoncxx::document::value make_branch(Branch branch);
  int upsert_branch(Branch branch) override;
  int upsert_branch(std::vector<Branch> branches) override;
  int upsert_branch_with_version(Branch branch, enum request_type type) override;
  int upsert_branch_with_version(std::vector<Branch> branches, enum request_type type) override;
  std::vector<std::string> list_branches_random(enum request_type type) override;

  bsoncxx::document::value make_commit(Commit commit);
  int upsert_commit(Commit commit) override;
  int upsert_commit(std::vector<Commit> commits) override;
  int upsert_commit_with_version(Commit commit, enum request_type type) override;
  int upsert_commit_with_version(std::vector<Commit> commits, enum request_type type) override;

  int insert_trending(const Trending &trending, std::string date) override;
};

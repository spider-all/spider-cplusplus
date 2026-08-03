#include <iostream>
#include <map>
#include <random>
#include <thread>
#include <utility>

#include <fmt/core.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#include <common.h>
#include <const.h>
#include <database.h>
#include <error.h>
#include <versions.h>

#pragma once

class SQLiteDatabase : public Database {
private:
  std::string db_path;
  SQLite::Database *db{};

  Versions *versions;

  const int32_t sample_size = 100;

  int execute(const std::string &sql);
  std::string escape(const std::string &s);

public:
  explicit SQLiteDatabase(const std::string &path);
  ~SQLiteDatabase() override;
  int initialize() override;
  int initialize_version() override;

  int64_t count_x(const std::string &c);
  std::vector<std::string> list_x_random(const std::string &collection, std::string keys, enum request_type type);

  int ensure_index(const std::string &collection, std::vector<std::string> keys);
  int create_x_collection(const std::string &collection, std::string keys);

  int create_collections();

  int update_version(std::string key, enum request_type type) override;
  int update_version(std::vector<std::string> keys, enum request_type type) override;
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

  int upsert_repo(Repo repo) override;
  int upsert_repo(std::vector<Repo> repos) override;
  int upsert_repo_with_version(Repo repo, enum request_type type) override;
  int upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) override;
  std::vector<std::string> list_repos_random(enum request_type type) override;
  int64_t count_repo() override;
};

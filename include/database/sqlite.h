#include <iostream>
#include <map>
#include <random>
#include <thread>
#include <utility>

#include <SQLiteCpp/SQLiteCpp.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <common.h>
#include <const.h>
#include <database.h>
#include <error.h>

#pragma once

class SQLiteDatabase : public Database {
private:
  std::string db_path;
  SQLite::Database *db{};

  const int32_t sample_size = 100;

  int execute(const std::string &sql);
  std::string escape(const std::string &s);
  int64_t current_timestamp() const;
  int64_t min_data_version(const std::string &collection);
  int update_data_version(const std::string &collection, const std::string &key_column, const std::string &key_value, int64_t version);
  int upsert_relation(const std::string &collection, const std::string &first_column, int64_t first_id, const std::string &second_column, int64_t second_id);

public:
  explicit SQLiteDatabase(const std::string &path);
  ~SQLiteDatabase() override;
  int initialize() override;

  int64_t count_x(const std::string &c);
  std::vector<std::string> list_x_random(const std::string &collection, std::string keys);

  int ensure_index(const std::string &collection, std::vector<std::string> keys);
  int create_x_collection(const std::string &collection, std::string keys);

  int create_collections();

  int upsert_user(User user) override;
  int upsert_user_with_version(User user, enum request_type type) override;
  int64_t count_user() override;
  std::vector<std::string> list_users_random() override;
  std::vector<User> list_usersx(common_args args) override;

  int upsert_org(Org org) override;
  int upsert_org(std::vector<Org> orgs) override;
  int upsert_org_with_version(Org org, enum request_type type) override;
  int upsert_org_with_version(std::vector<Org> orgs, enum request_type type) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs_random() override;

  int upsert_repo(Repo repo) override;
  int upsert_repo(std::vector<Repo> repos) override;
  int upsert_repo_with_version(Repo repo, enum request_type type) override;
  int upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) override;
  std::vector<std::string> list_repos_random() override;
  int64_t count_repo() override;

  int upsert_following(int64_t upstream_user_id, int64_t downstream_user_id) override;
  int upsert_starred(int64_t user_id, int64_t repo_id) override;
};

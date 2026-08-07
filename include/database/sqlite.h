#include <chrono>
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
  void log_query(const std::string &sql, std::chrono::steady_clock::time_point start) const;
  static std::string format_sql_cost(int64_t microseconds);
  std::string escape(const std::string &s);
  int64_t current_timestamp() const;
  int64_t min_data_version(const std::string &collection);
  int64_t initial_data_version(const std::string &collection);
  int update_data_version(const std::string &collection, const std::string &key_column, const std::string &key_value, int64_t version);

public:
  explicit SQLiteDatabase(const std::string &path);
  ~SQLiteDatabase() override;
  int initialize() override;
  int update_version_if_recent(const std::string &collection,
                               const std::string &key_column,
                               const std::string &key_value,
                               int64_t max_age_seconds,
                               bool &updated,
                               int64_t &remaining_seconds) override;

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
};

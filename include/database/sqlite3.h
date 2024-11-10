#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database.h>
#include <error.h>
#include <model.h>

class SQLite3 : public Database {
private:
  std::string path;
  SQLite::Database *db;

public:
  explicit SQLite3(const std::string &);
  ~SQLite3() override;
  int initialize() override;
  int initialize_version() override;

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

  int upsert_repo(Repo repo) override;
  int upsert_repo(std::vector<Repo> repos) override;
  int upsert_repo_with_version(Repo repo, enum request_type type) override;
  int upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) override;
  std::vector<std::string> list_repos_random(enum request_type type) override;
  int64_t count_repo() override;

  int upsert_branch(Branch branch) override;
  int upsert_branch(std::vector<Branch> branches) override;
  int upsert_branch_with_version(Branch branch, enum request_type type) override;
  int upsert_branch_with_version(std::vector<Branch> branches, enum request_type type) override;
  std::vector<std::string> list_branches_random(enum request_type type) override;

  int upsert_commit(Commit commit) override;
  int upsert_commit(std::vector<Commit> commits) override;
  int upsert_commit_with_version(Commit commit, enum request_type type) override;
  int upsert_commit_with_version(std::vector<Commit> commits, enum request_type type) override;
};

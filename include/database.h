#include <iostream>

#include <model.h>

#pragma once

class Database {
public:
  int code = 0;
  virtual ~Database() = default;
  virtual int initialize() = 0;

  virtual int upsert_user(User user) = 0;
  virtual int upsert_user_with_version(User user, enum request_type type) = 0;
  virtual int64_t count_user() = 0;
  virtual std::vector<std::string> list_users_random() = 0;

  virtual std::vector<User> list_usersx(common_args args) = 0;

  virtual int upsert_org(Org org) = 0;
  virtual int upsert_org(std::vector<Org> orgs) = 0;
  virtual int upsert_org_with_version(Org org, enum request_type type) = 0;
  virtual int upsert_org_with_version(std::vector<Org> orgs, enum request_type type) = 0;
  virtual int64_t count_org() = 0;
  virtual std::vector<std::string> list_orgs_random() = 0;

  virtual int upsert_repo(Repo repo) = 0;
  virtual int upsert_repo(std::vector<Repo> repos) = 0;
  virtual int upsert_repo_with_version(Repo repo, enum request_type type) = 0;
  virtual int upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) = 0;
  virtual std::vector<std::string> list_repos_random() = 0;
  virtual int64_t count_repo() = 0;

  virtual int upsert_following(int64_t upstream_user_id, int64_t downstream_user_id) = 0;
  virtual int upsert_starred(int64_t user_id, int64_t repo_id) = 0;
};

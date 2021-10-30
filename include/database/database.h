#include <iostream>

#pragma once

#include <model.h>

class Database {
public:
  int code = 0;
  virtual ~Database() = default;
  virtual int initialize() = 0;
  virtual int initialize_version() = 0;

  virtual int update_version(std::string key, enum request_type type) = 0;
  virtual int update_version(std::vector<std::string> key, enum request_type type) = 0;
  virtual int incr_version(enum request_type type) = 0;

  virtual int upsert_user(User user) = 0;
  virtual int upsert_user_with_version(User user, enum request_type type) = 0;
  virtual int64_t count_user() = 0;
  virtual std::vector<std::string> list_users_random(enum request_type type) = 0;

  virtual std::vector<User> list_usersx(common_args args) = 0;

  virtual int upsert_org(Org org) = 0;
  virtual int upsert_org_with_version(Org org, enum request_type type) = 0;
  virtual int64_t count_org() = 0;
  virtual std::vector<std::string> list_orgs_random(enum request_type type) = 0;

  virtual int create_emoji(std::vector<Emoji> emojis) = 0;
  virtual int64_t count_emoji() = 0;

  // virtual int create_gitignore(Gitignore gitignore) = 0;
  virtual int upsert_gitignore(Gitignore gitignore) = 0;
  virtual int64_t count_gitignore() = 0;

  virtual int upsert_license(License license) = 0;
  virtual int upsert_license_with_version(License license, enum request_type type) = 0;
  virtual int64_t count_license() = 0;

  virtual int create_repo(Repo repo, enum request_type type) = 0;
  virtual int64_t count_repo() = 0;
};

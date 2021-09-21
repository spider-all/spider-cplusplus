#include <iostream>

#pragma once

#include <model.h>

class Database {
public:
  int code = 0;
  virtual ~Database() = default;
  virtual int initialize() = 0;

  virtual int create_user(User) = 0;
  virtual int64_t count_user() = 0;
  virtual std::vector<std::string> list_users() = 0;

  virtual std::vector<User> list_usersx(common_args args) = 0;

  virtual int create_org(Org) = 0;
  virtual int64_t count_org() = 0;
  virtual std::vector<std::string> list_orgs() = 0;

  virtual int create_emoji(std::vector<Emoji> emojis) = 0;
  virtual int64_t count_emoji() = 0;

  virtual int create_gitignore(Gitignore gitignore) = 0;
  virtual int64_t count_gitignore() = 0;

  virtual int create_license(License license) = 0;
  virtual int64_t count_license() = 0;

  virtual int create_repo(Repo repo) = 0;
  virtual int64_t count_repo() = 0;
};

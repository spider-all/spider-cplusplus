#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>
#include <error.h>
#include <model.h>

class SQLite3 : public Database {
public:
  explicit SQLite3(const std::string &);
  ~SQLite3() override;
  int initialize() override;

  int64_t count_x(const std::string &table, const std::string &field);

  int create_user(User) override;
  std::vector<std::string> list_users() override;
  int64_t count_user() override;
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
};

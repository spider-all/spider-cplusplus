#include <iostream>
#include <random>
#include <thread>

#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

#include <error.h>

#include <database/database.h>

#pragma once

class MySQL : public Database {
private:
public:
  MySQL(const std::string &host, const std::string &user,
        const std::string &password, const std::string &db, unsigned int port);
  ~MySQL() override;
  int initialize() override;

  int create_user(User) override;
  int64_t count_user() override;
  std::vector<std::string> list_users() override;
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

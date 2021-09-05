#include <iostream>
#include <random>
#include <thread>

#include <hiredis/hiredis.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class Redis : public Database {
private:
  int set_value(const std::string &, const std::string &);
  int get_value(const std::string &, std::string *);
  std::mutex mtx;

public:
  Redis(const std::string &, int);
  ~Redis() override;
  int initialize() override;

  int create_user(User) override;
  int64_t count_user() override;
  std::vector<std::string> list_users() override;

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

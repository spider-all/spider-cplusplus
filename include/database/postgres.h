#include <iostream>
#include <random>
#include <thread>

#include <pqxx/except>
#include <pqxx/pqxx>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>

class Postgres : public Database {
private:
public:
  Postgres(const std::string &);
  ~Postgres() override;
  int initialize() override;

  int create_user(User) override;
  int64_t count_user() override;
  std::vector<std::string> list_users() override;

  int create_org(Org) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs() override;

  int create_emoji(std::vector<Emoji> emojis) override;
  int64_t count_emoji() override;
};

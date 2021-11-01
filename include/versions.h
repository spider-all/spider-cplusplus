#include <iostream>

#include <mongocxx/client.hpp>
#include <spdlog/spdlog.h>

#include <model.h>

#pragma once

class Versions {
private:
  int64_t followers_version = 1;
  int64_t following_version = 1;
  int64_t users_repos_version = 1;
  int64_t orgs_version = 1;
  int64_t orgs_member_version = 1;
  int64_t orgs_repos_version = 1;
  int64_t gitignore_list_version = 1;
  int64_t license_list_version = 1;

public:
  int64_t get(enum request_type type);
  int64_t incr(enum request_type type);
  std::string to_string(enum request_type type);

  int initialize(mongocxx::cursor cursor);
};

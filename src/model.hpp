#include <iostream>

#pragma once

struct github_user {
  int64_t id;
  std::string login;
  std::string node_id;
  std::string type;
  std::string name;
  std::string company;
  std::string blog;
  std::string location;
  std::string email;
  bool hireable;
  std::string bio;
  std::string created_at;
  std::string updated_at;
  int public_gists;
  int public_repos;
  int following;
  int followers;
};

const std::string CreateSentence[] = {
    "CREATE TABLE IF NOT EXISTS `users` ("
    "`id` NUMERIC NOT NULL PRIMARY KEY, "
    "`login` TEXT NOT NULL, "
    "`node_id` TEXT NOT NULL, "
    "`type` TEXT NOT NULL, "
    "`name` TEXT NOT NULL, "
    "`company` TEXT, "
    "`blog` TEXT, "
    "`location` TEXT, "
    "`email` TEXT, "
    "`hireable` NUMERIC, "
    "`bio` TEXT, "
    "`created_at` TEXT NOT NULL, "
    "`updated_at` TEXT NOT NULL, "
    "`public_gists` NUMERIC NOT NULL, "
    "`public_repos` NUMERIC NOT NULL, "
    "`following` NUMERIC NOT NULL, "
    "`followers` NUMERIC NOT NULL);",
};

const std::string QuerySentence[] = {
    "CREATE TABLE `followers` ("
    "`id` NUMERIC,"
    "`login` TEXT,"
    "`node_id` TEXT,"
    "`type` TEXT,"
    "`created_at` TEXT,"
    "`updated_at` TEXT);",
};

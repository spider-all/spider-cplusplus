#include <iostream>

#pragma once

typedef struct {
  unsigned long long page;
  unsigned long long limit;
} common_args;

typedef struct User {
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
} User;

typedef struct Org {
  int64_t id;
  std::string login;
  std::string node_id;
  std::string description;
} Org;

typedef struct Emoji {
  std::string name;
  std::string url;
} Emoji;

typedef struct Gitignore {
  std::string name;
  std::string source;
} Gitignore;

typedef struct License {
  std::string key;
  std::string name;
  std::string spdx_id;
  std::string node_id;
  std::string description;
  std::string implementation;
  std::string permissions;
  std::string conditions;
  std::string limitations;
  std::string body;
  bool featured;
} License;

typedef struct Repo {
  int64_t id;
  std::string node_id;
  std::string name;
  std::string full_name;
  bool xprivate;     // private
  std::string owner; // owner login
  std::string description;
  bool fork;
  std::string created_at;
  std::string updated_at;
  std::string pushed_at;
  std::string homepage;
  int64_t size;
  int64_t stargazers_count;
  int64_t watchers_count;
  int64_t forks_count;
  std::string language;
  std::string license; // license spdx
  int64_t forks;
  int64_t open_issues;
  int64_t watchers;
  std::string default_branch;
  int64_t network_count;
  int64_t subscribers_count;
  std::string organization; // organization login
} Repo;

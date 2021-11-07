#include <iostream>

#pragma once

enum request_type {
  request_type_followers,
  request_type_following,
  request_type_user,
  request_type_orgs,
  request_type_orgs_member,
  request_type_users_repos,
  request_type_users_repos_branches,
  request_type_orgs_repos,
  request_type_emoji,
  request_type_gitignore_list,
  request_type_gitignore_info,
  request_type_license_list,
  request_type_license_info,
};

typedef struct {
  long long page;
  long long limit;
  std::string query;
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
  int64_t public_gists;
  int64_t public_repos;
  int64_t following;
  int64_t followers;
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
  bool xprivate;          // private
  std::string owner;      // owner login
  std::string owner_type; // owner type
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
  std::string license; // license key
  int64_t forks;
  int64_t open_issues;
  int64_t watchers;
  std::string default_branch;
} Repo;

typedef struct Branch {
  std::string name;
  std::string commit;
  std::string repo;
} Branch;

typedef struct Trending {
  std::string seq;
  std::string spoken_language;
  std::string language;
  std::string owner;
  std::string repo;
  int64_t star;
} Trending;

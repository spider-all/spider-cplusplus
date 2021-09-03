#include <iostream>

#pragma once

struct user {
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

struct Org {
  int64_t id;
  std::string login;
  std::string node_id;
  std::string description;
};

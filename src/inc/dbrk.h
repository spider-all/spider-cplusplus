#include <database.h>

#pragma once

class DBRK : public Database {
public:
  DBRK(std::string);
  int initialize();
  void deinit();
  int create_user(user);
};

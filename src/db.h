#include <iostream>

#include <sqlite3.h>

class Database {
public:
  struct db {
    sqlite3 *sqlite;
  };
  virtual int initialize(std::string) = 0;
  virtual void deinit()               = 0;
};

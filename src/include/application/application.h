#pragma once

#include <config.h>
#include <database/database.h>

class Application {
public:
  virtual ~Application()= default;;
  virtual int startup() = 0;
};

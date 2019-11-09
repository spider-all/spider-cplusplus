#pragma once

class Application {
public:
  virtual int startup()   = 0;
  virtual void teardown() = 0;
};

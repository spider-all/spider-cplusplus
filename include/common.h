#pragma once

#define WRAP_FUNC(func) \
  {                     \
    int code = func;    \
    if (code != 0) {    \
      return code;      \
    }                   \
  }

class Common {
private:
public:
};

#include <iostream>

#pragma once

#define WRAP_FUNC(func) \
  {                     \
    int code = func;    \
    if (code != 0) {    \
      return code;      \
    }                   \
  }

class Common {
public:
  static bool end_with(std::string const &, std::string const &);
};

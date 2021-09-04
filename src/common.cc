#include <common.h>

bool Common::end_with(std::string const &str, std::string const &suffix) {
  if (str.length() >= suffix.length()) {
    return (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix));
  } else {
    return false;
  }
}

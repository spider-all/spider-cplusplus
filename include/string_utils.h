#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#pragma once

inline bool string_contains(const std::string &s, const std::string &needle) {
  return s.find(needle) != std::string::npos;
}

inline bool string_ends_with(const std::string &s, const std::string &suffix) {
  return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string string_join(const std::vector<std::string> &items, const std::string &delimiter) {
  std::ostringstream joined;
  for (size_t i = 0; i < items.size(); i++) {
    if (i > 0) {
      joined << delimiter;
    }
    joined << items[i];
  }
  return joined.str();
}

inline std::vector<std::string> string_split(const std::string &s, char delimiter) {
  std::vector<std::string> result;
  size_t start = 0;
  while (start <= s.size()) {
    size_t end = s.find(delimiter, start);
    if (end == std::string::npos) {
      result.push_back(s.substr(start));
      break;
    }
    result.push_back(s.substr(start, end - start));
    start = end + 1;
  }
  return result;
}

inline void string_trim(std::string &s) {
  auto is_not_space = [](unsigned char c) { return !std::isspace(c); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_space));
  s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
}

inline void string_to_lower(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
}

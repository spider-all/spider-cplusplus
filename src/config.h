#include <iostream>

#pragma once

class Config {
private:
public:
  std::string database_path;
  std::string crawler_entry_username; // entry user name
  std::string crawler_client_id;      // client id
  std::string crawler_client_secret;  // client secret

  int config(char *config_path);
};

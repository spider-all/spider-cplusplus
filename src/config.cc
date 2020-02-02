#include <config.h>

int Config::config(char *config_path) {
  YAML::Node config      = YAML::LoadFile(config_path);
  crawler_entry_username = config["name"].as<std::string>();
  crawler_token          = config["token"].as<std::string>();
  database_path          = config["database"]["path"].as<std::string>();
  database_type          = config["database"]["type"].as<std::string>();

  if (crawler_entry_username == "" || crawler_token == "") {
    spdlog::error("Config {0} have not the import value.", config_path);
    return CONFIG_PARSE_ERROR;
  }

  return 0;
}
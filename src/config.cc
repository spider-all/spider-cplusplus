#include <config.h>

int Config::initialize(const std::string &config_path) {
  try {
    YAML::Node config = YAML::LoadFile(config_path);
    crawler_entry_username = config["entry"].as<std::string>();
    crawler_token = config["token"].as<std::vector<std::string>>();
    crawler_useragent = config["useragent"].as<std::string>();
    crawler_timezone = config["timezone"].as<std::string>();
    crawler_sleep_each_request = config["sleep"].as<int64_t>();

    database_type = config["database"]["type"].as<std::string>();
    if (database_type == DATABASE_MONGODB) {
      database_mongodb_dsn = config["database"][DATABASE_MONGODB]["dsn"].as<std::string>();
    }

    auto crawler = config["crawler"];
    if (crawler) {
      if (crawler["followers"]) {
        this->crawler_type_followers = crawler["followers"].as<bool>();
      }
      if (crawler["followings"]) {
        this->crawler_type_followings = crawler["followings"].as<bool>();
      }
      if (crawler["orgs"]) {
        this->crawler_type_orgs = crawler["orgs"].as<bool>();
      }
      if (crawler["orgs_member"]) {
        this->crawler_type_orgs_member = crawler["orgs_member"].as<bool>();
      }
      if (crawler["users_repos"]) {
        this->crawler_type_users_repos = crawler["users_repos"].as<bool>();
      }
      if (crawler["orgs_repos"]) {
        this->crawler_type_orgs_repos = crawler["orgs_repos"].as<bool>();
      }
      if (crawler["emojis"]) {
        this->crawler_type_emojis = crawler["emojis"].as<bool>();
      }
      if (crawler["gitignore_list"]) {
        this->crawler_type_gitignore_list = crawler["gitignore_list"].as<bool>();
      }
      if (crawler["license_list"]) {
        this->crawler_type_license_list = crawler["license_list"].as<bool>();
      }
    }

    if (crawler_entry_username.empty() || crawler_token.empty()) {
      spdlog::error("Config {0} have not the import value.", config_path);
      return CONFIG_PARSE_ERROR;
    }
  } catch (const std::exception &e) {
    spdlog::error("Config parse with error: {0}", e.what());
    return CONFIG_PARSE_ERROR;
  }

  return 0;
}

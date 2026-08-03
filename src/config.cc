#include <config.h>

#include <string_utils.h>

int Config::initialize(const std::string &config_path) {
  try {
    YAML::Node config = YAML::LoadFile(config_path);
    crawler_entry_username = config["entry"].as<std::string>();
    if (config["token"]) {
      crawler_token = config["token"].as<std::vector<std::string>>();
    }
    std::string token_env = this->getenv("TOKEN");
    if (crawler_token.size() == 0 && token_env.size() > 0) {
      crawler_token = string_split(token_env, ',');
    }
    crawler_useragent = config["useragent"].as<std::string>();
    crawler_timezone = config["timezone"].as<std::string>();
    if (config["sleep_each_request"]) {
      crawler_sleep_each_request = config["sleep_each_request"].as<int64_t>();
    }
    if (crawler_sleep_each_request == 0) {
      crawler_sleep_each_request = DEFAULT_SLEEP_EACH_REQUEST;
    }

    if (config["database"] && config["database"][DATABASE_SQLITE] && config["database"][DATABASE_SQLITE]["path"]) {
      database_sqlite_path = config["database"][DATABASE_SQLITE]["path"].as<std::string>();
    }
    if (database_sqlite_path == "") {
      database_sqlite_path = this->getenv("DATABASE_SQLITE_PATH");
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
      if (crawler["events"]) {
        this->crawler_type_events = crawler["events"].as<bool>();
      }
    }

    if (crawler_entry_username.empty() || crawler_token.empty()) {
      spdlog::error("Config {0} or env have not the import value(entry username or crawler token).", config_path);
      return CONFIG_PARSE_ERROR;
    }
  } catch (const std::exception &e) {
    spdlog::error("Config parse with error: {0}", e.what());
    return CONFIG_PARSE_ERROR;
  }

  return 0;
}

std::string Config::getenv(const std::string &key) {
  char *value = std::getenv(key.c_str());
  if (value) {
    return std::string(value);
  }
  return "";
}

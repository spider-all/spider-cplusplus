#include <config.h>

int Config::initialize(const std::string &config_path) {
  try {
    YAML::Node config = YAML::LoadFile(config_path);
    crawler_entry_username = config["entry"].as<std::string>();
    if (config["token"]) {
      crawler_token = config["token"].as<std::string>();
    }
    if (crawler_token.empty()) {
      crawler_token = this->getenv("TOKEN");
    }
    crawler_useragent = config["useragent"].as<std::string>();
    crawler_timezone = config["timezone"].as<std::string>();
    if (config["sleep_each_request"]) {
      crawler_sleep_each_request = config["sleep_each_request"].as<int64_t>();
    }
    if (crawler_sleep_each_request == 0) {
      crawler_sleep_each_request = DEFAULT_SLEEP_EACH_REQUEST;
    }

    if (config["database"] && config["database"]["path"]) {
      database_sqlite_path = config["database"]["path"].as<std::string>();
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
      if (crawler["starred"]) {
        this->crawler_type_starred = crawler["starred"].as<bool>();
      }
      if (crawler["events"]) {
        this->crawler_type_events = crawler["events"].as<bool>();
      }
    }

    auto repositories = config["repositories"];
    if (repositories) {
      if (repositories["trend_languages"]) {
        this->repository_trend_languages = repositories["trend_languages"].as<std::vector<std::string>>();
      }
      if (repositories["refresh_interval_minutes"]) {
        this->repository_refresh_interval_minutes = repositories["refresh_interval_minutes"].as<int64_t>();
      }
    }
    if (this->repository_refresh_interval_minutes <= 0) {
      this->repository_refresh_interval_minutes = 10;
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

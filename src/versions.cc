#include <versions.h>

int Versions::initialize(SQLite::Database &db) {
  try {
    SQLite::Statement query(db, "SELECT type, version FROM versions");
    while (query.executeStep()) {
      SQLite::Column type_val = query.getColumn(0);
      SQLite::Column version_val = query.getColumn(1);
      if (type_val.isNull() || version_val.isNull()) {
        continue;
      }
      std::string type_string = type_val.getString();
      int64_t ver = version_val.getInt64();
      if (type_string == this->to_string(request_type_followers)) {
        this->followers_version = ver;
      } else if (type_string == this->to_string(request_type_following)) {
        this->following_version = ver;
      } else if (type_string == this->to_string(request_type_orgs)) {
        this->orgs_version = ver;
      } else if (type_string == this->to_string(request_type_orgs_member)) {
        this->orgs_member_version = ver;
      } else if (type_string == this->to_string(request_type_users_repos)) {
        this->users_repos_version = ver;
      } else if (type_string == this->to_string(request_type_orgs_repos)) {
        this->orgs_repos_version = ver;
      } else if (type_string == this->to_string(request_type_events)) {
        this->events_version = ver;
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("SQLite error: {}", e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int64_t Versions::get(enum request_type type) {
  int64_t version = 0;
  if (type == request_type_followers) {
    version = this->followers_version;
  } else if (type == request_type_following) {
    version = this->following_version;
  } else if (type == request_type_orgs) {
    version = this->orgs_version;
  } else if (type == request_type_orgs_member) {
    version = this->orgs_member_version;
  } else if (type == request_type_orgs_repos) {
    version = this->orgs_repos_version;
  } else if (type == request_type_users_repos) {
    version = this->users_repos_version;
  } else if (type == request_type_events) {
    version = this->events_version;
  } else {
    spdlog::error("unknown request type in Versions::get: {} ({})", static_cast<int>(type), request_type_name(type));
  }
  return version;
}

int64_t Versions::incr(enum request_type type) {
  int64_t version = 0;
  if (type == request_type_followers) {
    this->followers_version++;
    version = this->followers_version;
  } else if (type == request_type_following) {
    this->following_version++;
    version = this->following_version;
  } else if (type == request_type_orgs) {
    this->orgs_version++;
    version = this->orgs_version;
  } else if (type == request_type_orgs_member) {
    this->orgs_member_version++;
    version = this->orgs_member_version;
  } else if (type == request_type_orgs_repos) {
    this->orgs_repos_version++;
    version = this->orgs_repos_version;
  } else if (type == request_type_users_repos) {
    this->users_repos_version++;
    version = this->users_repos_version;
  } else if (type == request_type_events) {
    this->events_version++;
    version = this->events_version;
  } else {
    spdlog::error("unknown request type in Versions::incr: {} ({})", static_cast<int>(type), request_type_name(type));
  }
  return version;
}

std::string Versions::to_string(enum request_type type) {
  switch (type) {
  case request_type_followers:
    return "followers";
  case request_type_following:
    return "following";
  case request_type_orgs:
    return "orgs";
  case request_type_orgs_member:
    return "orgs_member";
  case request_type_orgs_repos:
    return "orgs_repos";
  case request_type_users_repos:
    return "users_repos";
  case request_type_events:
    return "events";
  default:
    spdlog::error("unknown request type in Versions::to_string: {} ({})", static_cast<int>(type), request_type_name(type));
    return "Unknown type";
  }
}

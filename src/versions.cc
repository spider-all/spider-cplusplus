#include <versions.h>

int Versions::initialize(mongocxx::cursor cursor) {
  for (auto &&doc : cursor) {
    bsoncxx::document::element type = doc["type"];
    bsoncxx::document::element version = doc["version"];
    try {
      if ((type && type.type() == bsoncxx::type::k_utf8) &&
          (version && version.type() == bsoncxx::type::k_int64)) {
        std::string type_string(type.get_string().value);
        if (type_string == this->to_string(request_type_followers)) {
          this->followers_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_following)) {
          this->following_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_orgs)) {
          this->orgs_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_orgs_member)) {
          this->orgs_member_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_users_repos)) {
          this->users_repos_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_orgs_repos)) {
          this->orgs_repos_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_gitignore_list)) {
          this->gitignore_list_version = version.get_int64().value;
        } else if (type_string == this->to_string(request_type_license_list)) {
          this->license_list_version = version.get_int64().value;
        }
      }
    } catch (const std::exception &e) {
      spdlog::error("initialize versions with error: {}", e.what());
      return EXIT_FAILURE;
    }
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
  } else if (type == request_type_gitignore_list) {
    version = this->gitignore_list_version;
  } else if (type == request_type_license_list) {
    version = this->license_list_version;
  } else if (type == request_type_users_repos) {
    version = this->users_repos_version;
  } else if (type == request_type_users_repos_branches) {
    version = this->users_repos_branches_version;
  } else {
    spdlog::error("unknown request type {}", static_cast<int>(type));
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
  } else if (type == request_type_gitignore_list) {
    this->gitignore_list_version++;
    version = this->gitignore_list_version;
  } else if (type == request_type_license_list) {
    this->license_list_version++;
    version = this->license_list_version;
  } else if (type == request_type_users_repos) {
    this->users_repos_version++;
    version = this->users_repos_version;
  } else if (type == request_type_users_repos_branches) {
    this->users_repos_branches_version++;
    version = this->users_repos_branches_version;
  } else {
    spdlog::error("unknown request type {}", static_cast<int>(type));
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
  case request_type_gitignore_list:
    return "gitignore";
  case request_type_license_list:
    return "license";
  case request_type_users_repos:
    return "users_repos";
  case request_type_users_repos_branches:
    return "users_repos_branches";
  default:
    spdlog::error("unknown request type {}", static_cast<int>(type));
    return "Unknown type";
  }
}

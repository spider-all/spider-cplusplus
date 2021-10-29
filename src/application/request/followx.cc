#include <application/request.h>

int Request::startup_followx() {
  if (config.crawler_type_followers) {
    semaphore++;
    std::thread followers_thread([=]() {
      spdlog::info("Followers thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_users_random(request_type_followers);
        for (const std::string &u : users) {
          std::string request_url = "/users/" + u + "/followers";
          int code = request(request_url, request_type_followers, request_type_followers);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_url, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("Followers thread stopped");
      semaphore--;
    });
    followers_thread.detach();
  }
  if (config.crawler_type_followings) {
    semaphore++;
    std::thread followings_thread([=]() {
      spdlog::info("Following thread is starting...");
      while (!stopping) {
        std::vector<std::string> users = database->list_users_random(request_type_following);
        for (const std::string &u : users) {
          std::string request_url = "/users/" + u + "/following";
          int code = request(request_url, request_type_following, request_type_following);
          if (code != 0) {
            spdlog::error("Request url: {} with error: {}", request_url, code);
          }
          if (stopping) {
            break;
          }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      spdlog::info("Following thread stopped");
      semaphore--;
    });
    followings_thread.detach();
  }
  return EXIT_SUCCESS;
}

int Request::request_user(nlohmann::json content, enum request_type type_from) {
  User user;
  if (content["hireable"].dump() != "true") {
    content["hireable"] = false;
  }
  user.login = content["login"].get<std::string>();
  user.id = content["id"].get<int64_t>();
  user.node_id = content["node_id"].get<std::string>();
  user.type = content["type"].get<std::string>();
  user.name = content["name"].get<std::string>();
  user.company = content["company"].get<std::string>();
  user.blog = content["blog"].get<std::string>();
  user.location = content["location"].get<std::string>();
  user.email = content["email"].get<std::string>();
  user.hireable = content["hireable"].get<bool>();
  user.bio = content["bio"].get<std::string>();
  user.created_at = content["created_at"].get<std::string>();
  user.updated_at = content["updated_at"].get<std::string>();
  user.public_gists = content["public_gists"].get<int>();
  user.public_repos = content["public_repos"].get<int>();
  user.following = content["following"].get<int>();
  user.followers = content["followers"].get<int>();

  int code = database->upsert_user_with_version(user, type_from);
  return code;
}

int Request::request_followx(const nlohmann::json &content, enum request_type type_from) {
  for (auto i : content) {
    int code = request("/users/" + i["login"].get<std::string>(), request_type_user, type_from);
    if (code != 0) {
      spdlog::error("Request userinfo with error: {}", code);
      return code;
    }
    if (stopping) {
      return EXIT_SUCCESS;
    }
  }
  return EXIT_SUCCESS;
}

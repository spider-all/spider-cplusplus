#include <database/redis.h>

DBRedis::DBRedis(const std::string &host, int port) {
  struct timeval timeout = {1, 500000};

  db.redis = redisConnectWithTimeout(host.c_str(), port, timeout);
}

DBRedis::~DBRedis() { redisFree(db.redis); }

int DBRedis::initialize() {
  if (db.redis == nullptr || db.redis->err) {
    redisFree(db.redis);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int DBRedis::create_user(user user) {
  std::string userId = std::to_string(user.id);

  int code = set_value("user:id:" + userId, userId);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:login:" + userId, user.login);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:node_id:" + userId, user.node_id);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:type:" + userId, user.type);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:name:" + userId, user.name);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:company:" + userId, user.company);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:blog:" + userId, user.blog);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:location:" + userId, user.location);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:email:" + userId, user.email);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:hireable:" + userId, user.hireable ? "1" : "0");
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:bio:" + userId, user.bio);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:created_at:" + userId, user.created_at);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:updated_at:" + userId, user.updated_at);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:public_gists:" + userId,
                   std::to_string(user.public_gists));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:public_repos:" + userId,
                   std::to_string(user.public_repos));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:following:" + userId, std::to_string(user.following));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:followers:" + userId, std::to_string(user.followers));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }

  return code == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int DBRedis::set_value(const std::string &key, const std::string &value) {
  auto *reply = (redisReply *)redisCommand(db.redis, "SET %s %s", key.c_str(),
                                           value.c_str());

  int code = 0;
  if (reply == nullptr) {
    spdlog::info("Redis got an error");
    code = 1;
  } else if (reply->type != REDIS_REPLY_STATUS ||
             strcasecmp(reply->str, "OK") != 0) {
    spdlog::error("Redis got error code: {}, {}", reply->type, reply->str);
    code = 1;
  }
  if (reply != nullptr) {
    freeReplyObject(reply);
  }
  return code == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int DBRedis::get_value(const std::string &key, std::string *value) {
  auto *reply = (redisReply *)redisCommand(db.redis, "GET %s", key.c_str());

  int code = 0;
  if (reply == nullptr) {
    spdlog::info("Redis got an error");
    code = 1;
  } else if (reply->type == REDIS_REPLY_STRING) {
    *value = std::string(reply->str);
  }
  if (reply != nullptr) {
    freeReplyObject(reply);
  }
  return code == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

std::vector<std::string> DBRedis::list_users() {
  mtx.lock();
  std::vector<std::string> users;
  auto *reply = (redisReply *)redisCommand(db.redis, "KEYS user:login:*");
  if (reply == nullptr) {
    spdlog::info("Redis got an error");
  } else if (reply->type == REDIS_REPLY_ARRAY) {
    for (size_t j = 0; j < reply->elements; j++) {
      std::string value;
      if (reply->element[j]->str != nullptr)
        get_value(std::string(reply->element[j]->str), &value);
      users.push_back(value);
    }
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::shuffle(
      users.begin(), users.end(),
      std::default_random_engine(
          std::chrono::system_clock::now().time_since_epoch().count()));
  if (reply != nullptr) {
    freeReplyObject(reply);
  }
  mtx.unlock();
  return users;
}

int DBRedis::count_user() { return 0; }

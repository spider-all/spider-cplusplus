#include <database/dbrd.h>

DBRD::DBRD(std::string host, int port) {
  struct timeval timeout = {1, 500000};

  db.redis = redisConnectWithTimeout(host.c_str(), port, timeout);
}

DBRD::~DBRD() {
  redisFree(db.redis);
}

int DBRD::initialize() {
  if (db.redis == NULL || db.redis->err) {
    redisFree(db.redis);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int DBRD::create_user(user user) {
  int code = set_value("user:id:" + std::to_string(user.id), std::to_string(user.id));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:login:" + std::to_string(user.id), user.login);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }

  return code == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int DBRD::set_value(std::string key, std::string value) {
  redisReply *reply = (redisReply *)redisCommand(db.redis, "SET %s %s", key.c_str(), value.c_str());

  int code = 0;
  if (reply == NULL) {
    spdlog::info("Redis got an error");
    code = 1;
  } else if (reply->type != REDIS_REPLY_STATUS || strcasecmp(reply->str, "OK") != 0) {
    spdlog::error("Redis got error code: {}, {}", reply->type, reply->str);
    code = 1;
  }
  if (reply != NULL) {
    freeReplyObject(reply);
  }
  return code == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int DBRD::get_value(std::string key, std::string *value) {
  redisReply *reply = (redisReply *)redisCommand(db.redis, "GET %s", key.c_str());

  int code = 0;
  if (reply == NULL) {
    spdlog::info("Redis got an error");
    code = 1;
  }
  if (reply->type == REDIS_REPLY_STRING) {
    *value = std::string(reply->str);
  }
  if (reply != NULL) {
    freeReplyObject(reply);
  }
  return code == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

std::vector<std::string> DBRD::list_users() {
  mtx.lock();
  std::vector<std::string> users;
  redisReply *reply = (redisReply *)redisCommand(db.redis, "KEYS user:login:*");
  if (reply == NULL) {
    spdlog::info("Redis got an error");
  } else if (reply->type == REDIS_REPLY_ARRAY) {
    for (int j = 0; j < reply->elements; j++) {
      std::string value;
      if (reply->element[j]->str != NULL) get_value(std::string(reply->element[j]->str), &value);
      users.push_back(value);
    }
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::shuffle(users.begin(), users.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
  if (reply != NULL) {
    freeReplyObject(reply);
  }
  mtx.unlock();
  return users;
}

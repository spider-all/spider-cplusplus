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
  std::string userid = std::to_string(user.id);

  int code = set_value("user:id:" + userid, userid);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:login:" + userid, user.login);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:node_id:" + userid, user.node_id);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:type:" + userid, user.type);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:name:" + userid, user.name);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:company:" + userid, user.company);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:blog:" + userid, user.blog);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:location:" + userid, user.location);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:email:" + userid, user.email);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:hireable:" + userid, user.hireable ? "1" : "0");
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:bio:" + userid, user.bio);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:created_at:" + userid, user.created_at);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:updated_at:" + userid, user.updated_at);
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:public_gists:" + userid, std::to_string(user.public_gists));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:public_repos:" + userid, std::to_string(user.public_repos));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:following:" + userid, std::to_string(user.following));
  if (code == EXIT_FAILURE) {
    return EXIT_SUCCESS;
  }
  code = set_value("user:followers:" + userid, std::to_string(user.followers));
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

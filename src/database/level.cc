#include <database/level.h>

Level::Level(const std::string &path) {
  leveldb::Options options;
  options.create_if_missing = true;

  status = leveldb::DB::Open(options, path, &db.leveldb);
}

Level::~Level() { delete db.leveldb; }

int Level::initialize() {
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Level::create_user(User user) {
  std::string userId = std::to_string(user.id);

  status = db.leveldb->Put(leveldb::WriteOptions(), "user:id:" + userId, userId);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:login:" + userId, user.login);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:node_id:" + userId, user.node_id);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:type:" + userId, user.type);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:name:" + userId, user.name);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:company:" + userId, user.company);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:location:" + userId, user.location);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:email:" + userId, user.email);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:hireable:" + userId, user.hireable ? "1" : "0");
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:bio:" + userId, user.bio);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:created_at:" + userId, user.created_at);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:updated_at:" + userId, user.updated_at);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:public_gists:" + userId, std::to_string(user.public_gists));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:public_repos:" + userId, std::to_string(user.public_repos));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:following:" + userId, std::to_string(user.following));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.leveldb->Put(leveldb::WriteOptions(), "user:followers:" + userId, std::to_string(user.followers));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int Level::create_org(Org) {
  return EXIT_SUCCESS;
}

std::vector<std::string> Level::list_users() {
  std::vector<std::string> users;
  leveldb::Iterator *iter = db.leveldb->NewIterator(leveldb::ReadOptions());
  for (iter->Seek("user:login:"); iter->Valid() && iter->key().starts_with("user:login:"); iter->Next()) {
    users.push_back(iter->value().ToString());
  }
  delete iter;

  std::shuffle(users.begin(), users.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

  if (users.size() > 100) {
    users.resize(100);
  }

  return users;
}

std::vector<std::string> Level::list_orgs() {
  std::vector<std::string> orgs;
  return orgs;
}

int64_t Level::count_user() { return 0; }
int64_t Level::count_org() { return 0; }

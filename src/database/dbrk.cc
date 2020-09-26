#include <database/dbrk.h>

DBRK::DBRK(const std::string &path) {
  leveldb::Options options;
  options.create_if_missing = true;
  status = leveldb::DB::Open(options, path, &db.rocksdb);
}

DBRK::~DBRK() {
  delete db.rocksdb;
}

int DBRK::initialize() {
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int DBRK::create_user(user user) {
  std::string userId = std::to_string(user.id);

  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:id:" + userId, userId);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:login:" + userId, user.login);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:node_id:" + userId, user.node_id);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:type:" + userId, user.type);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:name:" + userId, user.name);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:company:" + userId, user.company);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:location:" + userId, user.location);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:email:" + userId, user.email);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:hireable:" + userId, user.hireable ? "1" : "0");
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:bio:" + userId, user.bio);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:created_at:" + userId, user.created_at);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:updated_at:" + userId, user.updated_at);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:public_gists:" + userId, std::to_string(user.public_gists));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:public_repos:" + userId, std::to_string(user.public_repos));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:following:" + userId, std::to_string(user.following));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(leveldb::WriteOptions(), "user:followers:" + userId, std::to_string(user.followers));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

std::vector<std::string> DBRK::list_users() {
  std::vector<std::string> users;
  leveldb::Iterator *iter = db.rocksdb->NewIterator(leveldb::ReadOptions());
  for (iter->Seek("user:login:"); iter->Valid() && iter->key().starts_with("user:login:"); iter->Next()) {
    users.push_back(iter->value().ToString());
  }
  delete iter;

  std::shuffle(users.begin(), users.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
  return users;
}

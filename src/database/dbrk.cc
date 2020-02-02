#include <database/dbrk.h>

DBRK::DBRK(std::string path) {
  rocksdb::Options options;
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  options.create_if_missing = true;

  status = rocksdb::DB::Open(options, path, &db.rocksdb);
}

DBRK::~DBRK() {
  if (db.rocksdb != nullptr) {
    delete db.rocksdb;
  }
}

int DBRK::initialize() {
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int DBRK::create_user(user user) {
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:id:" + std::to_string(user.id), std::to_string(user.id));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:login:" + user.login, user.login);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

std::vector<std::string> DBRK::list_users() {
  std::vector<std::string> users;
  rocksdb::Iterator *iter = db.rocksdb->NewIterator(rocksdb::ReadOptions());
  for (iter->Seek("user:login:"); iter->Valid() && iter->key().starts_with("user:login:"); iter->Next()) {
    users.push_back(iter->value().ToString());
  }
  delete iter;

  std::shuffle(users.begin(), users.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
  return users;
}

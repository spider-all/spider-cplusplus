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
  std::string userid = std::to_string(user.id);

  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:id:" + userid, userid);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:login:" + userid, user.login);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:node_id:" + userid, user.node_id);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:type:" + userid, user.type);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:name:" + userid, user.name);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:company:" + userid, user.company);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:location:" + userid, user.location);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:email:" + userid, user.email);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:hireable:" + userid, user.hireable ? "1" : "0");
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:bio:" + userid, user.bio);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:created_at:" + userid, user.created_at);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:updated_at:" + userid, user.updated_at);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:public_gists:" + userid, std::to_string(user.public_gists));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:public_repos:" + userid, std::to_string(user.public_repos));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:following:" + userid, std::to_string(user.following));
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  status = db.rocksdb->Put(rocksdb::WriteOptions(), "user:followers:" + userid, std::to_string(user.followers));
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

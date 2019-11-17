#include <dbrk.h>

DBRK::DBRK(std::string path) {
  rocksdb::Options options;
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  options.create_if_missing = true;

  rocksdb::Status s = rocksdb::DB::Open(options, path, &db.rocksdb);
}

DBRK::~DBRK() {
  if (db.rocksdb != nullptr) {
    delete db.rocksdb;
  }
}

int DBRK::initialize() { return EXIT_SUCCESS; }

int DBRK::create_user(user user) { return EXIT_SUCCESS; }

std::vector<std::string> DBRK::list_users() {
  std::vector<std::string> users;
  return users;
}

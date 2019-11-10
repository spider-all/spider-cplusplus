#include <dbrk.h>

DBRK::DBRK(std::string path) {
  rocksdb::Options options;
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  options.create_if_missing = true;

  rocksdb::Status s = rocksdb::DB::Open(options, path, &db.rocksdb);
}
int DBRK::initialize() { return EXIT_SUCCESS; }
void DBRK::deinit() {
  if (db.rocksdb != nullptr) {
    delete db.rocksdb;
  }
}
int DBRK::create_user(user user) { return EXIT_SUCCESS; }

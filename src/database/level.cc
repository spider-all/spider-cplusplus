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

int64_t Level::count_x(const std::string& table, const std::string& field) {
  int64_t count = 0;
  std::string key = table + ":" + field;
  leveldb::Iterator *iter = db.leveldb->NewIterator(leveldb::ReadOptions());
  for (iter->Seek(key); iter->Valid() && iter->key().starts_with(key); iter->Next()) {
    count++;
  }
  delete iter;
  return count;
}

int Level::create_user(User user) {
  leveldb::WriteBatch batch;
  batch.Put("users:id:" + std::to_string(user.id), std::to_string(user.id));
  batch.Put("users:login:" + std::to_string(user.id), user.login);
  batch.Put("users:node_id:" + std::to_string(user.id), user.node_id);
  batch.Put("users:type:" + std::to_string(user.id), user.type);
  batch.Put("users:name:" + std::to_string(user.id), user.name);
  batch.Put("users:company:" + std::to_string(user.id), user.company);
  batch.Put("users:location:" + std::to_string(user.id), user.location);
  batch.Put("users:email:" + std::to_string(user.id), user.email);
  batch.Put("users:hireable:" + std::to_string(user.id), user.hireable ? "1" : "0");
  batch.Put("users:bio:" + std::to_string(user.id), user.bio);
  batch.Put("users:created_at:" + std::to_string(user.id), user.created_at);
  batch.Put("users:updated_at:" + std::to_string(user.id), user.updated_at);
  batch.Put("users:public_gists:" + std::to_string(user.id), std::to_string(user.public_gists));
  batch.Put("users:following:" + std::to_string(user.id), std::to_string(user.following));
  batch.Put("users:followers:" + std::to_string(user.id), std::to_string(user.followers));
  status = db.leveldb->Write(leveldb::WriteOptions(), &batch);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

std::vector<std::string> Level::list_users() {
  std::vector<std::string> users;
  leveldb::Iterator *iter = db.leveldb->NewIterator(leveldb::ReadOptions());
  for (iter->Seek("users:login:"); iter->Valid() && iter->key().starts_with("users:login:"); iter->Next()) {
    users.push_back(iter->value().ToString());
  }
  delete iter;

  if (users.size() > 100) {
    auto seed = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(users.begin(), users.end(), seed);
    users.resize(100);
  }

  return users;
}

int64_t Level::count_user() {
  return count_x("users", "id");
}

int Level::create_org(Org org) {
  leveldb::WriteBatch batch;
  batch.Put("orgs:id:" + std::to_string(org.id), std::to_string(org.id));
  batch.Put("orgs:login:" + std::to_string(org.id), org.login);
  batch.Put("orgs:node_id:" + std::to_string(org.id), org.node_id);
  batch.Put("orgs:description:" + std::to_string(org.id), org.description);
  status = db.leveldb->Write(leveldb::WriteOptions(), &batch);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

std::vector<User> Level::list_usersx(common_args args) {
    std::vector<User> users;
    return users;
}

std::vector<std::string> Level::list_orgs() {
  std::vector<std::string> orgs;
  leveldb::Iterator *iter = db.leveldb->NewIterator(leveldb::ReadOptions());
  std::string key = "orgs:login";
  for (iter->Seek(key); iter->Valid() && iter->key().starts_with(key); iter->Next()) {
    orgs.push_back(iter->value().ToString());
  }
  delete iter;

  if (orgs.size() > 100) {
    auto seed = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(orgs.begin(), orgs.end(), seed);
    orgs.resize(100);
  }
  return orgs;
}

int64_t Level::count_org() {
  return count_x("orgs", "id");
}

int Level::create_emoji(std::vector<Emoji> emojis) {
  leveldb::WriteBatch batch;
  for (const Emoji& emoji : emojis) {
    batch.Put("emojis:name:" + emoji.name, emoji.name);
    batch.Put("emojis:url:" + emoji.name, emoji.url);
  }
  status = db.leveldb->Write(leveldb::WriteOptions(), &batch);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int64_t Level::count_emoji() {
  return count_x("emojis", "name");
}

int Level::create_gitignore(Gitignore gitignore) {
  leveldb::WriteBatch batch;
  batch.Put("gitignores:name:" + gitignore.name, gitignore.name);
  batch.Put("gitignores:source:" + gitignore.name, gitignore.source);
  status = db.leveldb->Write(leveldb::WriteOptions(), &batch);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int64_t Level::count_gitignore() {
  return count_x("gitignores", "name");
}

int Level::create_license(License license) {
  leveldb::WriteBatch batch;
  batch.Put("licenses:key:" + license.key, license.key);
  batch.Put("licenses:name:" + license.key, license.name);
  batch.Put("licenses:spdx_id:" + license.key, license.spdx_id);
  batch.Put("licenses:node_id:" + license.key, license.node_id);
  batch.Put("licenses:description:" + license.key, license.description);
  batch.Put("licenses:implementation:" + license.key, license.implementation);
  batch.Put("licenses:permissions:" + license.key, license.permissions);
  batch.Put("licenses:conditions:" + license.key, license.conditions);
  batch.Put("licenses:limitations:" + license.key, license.limitations);
  batch.Put("licenses:body:" + license.key, license.body);
  batch.Put("licenses:featured:" + license.key, std::to_string(license.featured));
  status = db.leveldb->Write(leveldb::WriteOptions(), &batch);
  if (!status.ok()) {
    spdlog::info("Database got an error: {}", status.ToString());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int64_t Level::count_license() {
  return count_x("licenses", "key");
}

#include <database/mongo.h>

int Mongo::upsert_gitignore(Gitignore gitignore) {
  bsoncxx::document::value doc = make_document(
      kvp("name", gitignore.name),
      kvp("source", gitignore.source));
  bsoncxx::document::value filter = make_document(kvp("name", gitignore.name));
  return this->upsert_x("gitignores", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int64_t Mongo::count_gitignore() {
  return this->count_x("gitignores");
}

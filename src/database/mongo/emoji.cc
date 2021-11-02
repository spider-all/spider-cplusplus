#include <database/mongo.h>

int Mongo::upsert_emoji(std::vector<Emoji> emojis) {
  std::map<std::string, std::string> filters;
  std::vector<bsoncxx::document::value> docs;
  for (const Emoji &emoji : emojis) {
    bsoncxx::document::value record = make_document(kvp("name", emoji.name), kvp("url", emoji.url));
    bsoncxx::document::value filter = make_document(kvp("name", emoji.name));
    filters.insert(std::pair(bsoncxx::to_json(filter), bsoncxx::to_json(record)));
  }
  return this->upsert_x("emojis", filters);
}

int64_t Mongo::count_emoji() {
  return this->count_x("emojis");
}

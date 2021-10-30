#include <database/mongo.h>

int Mongo::create_emoji(std::vector<Emoji> emojis) {
  try {
    GET_CONNECTION(this->uri->database(), "emojis")
    coll.drop();
    std::vector<bsoncxx::document::value> docs;
    for (const Emoji &emoji : emojis) {
      bsoncxx::document::value doc = make_document(kvp("name", emoji.name), kvp("url", emoji.url));
      docs.push_back(doc);
    }
    coll.insert_many(docs);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int64_t Mongo::count_emoji() {
  return this->count_x("emojis");
}

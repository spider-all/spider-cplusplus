#include <database/mongo.h>

int Mongo::insert_trending(const Trending &trending, std::string date) {
  bsoncxx::document::value doc = make_document(
      kvp("date", date),
      kvp("seq", trending.seq),
      kvp("spoken_language", trending.spoken_language),
      kvp("language", trending.language),
      kvp("owner", trending.owner),
      kvp("repo", trending.repo),
      kvp("star", trending.star));
  return this->insert_x("trendings", doc.view());
}

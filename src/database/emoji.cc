#include <database/sqlite.h>

int SQLiteDatabase::upsert_emoji(std::vector<Emoji> emojis) {
  for (const auto &emoji : emojis) {
    std::string sql = fmt::format(
        "INSERT OR REPLACE INTO emojis (name, url) VALUES ('{}', '{}')",
        this->escape(emoji.name), this->escape(emoji.url));
    WRAP_FUNC(this->execute(sql))
  }
  return EXIT_SUCCESS;
}

int64_t SQLiteDatabase::count_emoji() {
  return this->count_x("emojis");
}
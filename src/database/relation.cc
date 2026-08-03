#include <database/sqlite.h>

int SQLiteDatabase::upsert_following(int64_t upstream_user_id, int64_t downstream_user_id) {
  return this->upsert_relation("user_following", "upstream_user_id", upstream_user_id, "downstream_user_id", downstream_user_id);
}

int SQLiteDatabase::upsert_starred(int64_t user_id, int64_t repo_id) {
  return this->upsert_relation("repo_star", "user_id", user_id, "repo_id", repo_id);
}

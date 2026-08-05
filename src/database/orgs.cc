#include <database/sqlite.h>

int SQLiteDatabase::upsert_org(Org org) {
  int64_t now = this->current_timestamp();
  int64_t version = this->initial_data_version("orgs");
  std::string sql = fmt::format(
      "INSERT INTO orgs (id, login, node_id, description, followers, data_created_at, data_updated_at, data_version) "
      "VALUES ({}, '{}', '{}', '{}', {}, {}, {}, {}) "
      "ON CONFLICT(id) DO UPDATE SET "
      "login = excluded.login, "
      "node_id = excluded.node_id, "
      "description = excluded.description, "
      "followers = excluded.followers, "
      "data_updated_at = {}",
      org.id,
      this->escape(org.login),
      this->escape(org.node_id),
      this->escape(org.description),
      org.followers,
      now,
      now,
      version,
      now);
  return this->execute(sql);
}

int SQLiteDatabase::upsert_org(std::vector<Org> orgs) {
  for (const auto &org : orgs) {
    WRAP_FUNC(this->upsert_org(org))
  }
  return EXIT_SUCCESS;
}

int SQLiteDatabase::upsert_org_with_version(Org org, enum request_type type) {
  WRAP_FUNC(this->upsert_org(org))
  return EXIT_SUCCESS;
}

int SQLiteDatabase::upsert_org_with_version(std::vector<Org> orgs, enum request_type type) {
  WRAP_FUNC(this->upsert_org(orgs))
  return EXIT_SUCCESS;
}

std::vector<std::string> SQLiteDatabase::list_orgs_random() {
  return this->list_x_random("orgs", "id;login");
}

int64_t SQLiteDatabase::count_org() {
  return this->count_x("orgs");
}

#include <database/sqlite.h>

int SQLiteDatabase::upsert_org(Org org) {
  std::string sql = fmt::format(
      "INSERT INTO orgs (id, login, node_id, description, followers, data_created_at, data_updated_at) "
      "VALUES ({}, '{}', '{}', '{}', {}, CAST(strftime('%s','now') AS INTEGER), CAST(strftime('%s','now') AS INTEGER)) "
      "ON CONFLICT(id) DO UPDATE SET "
      "login = excluded.login, "
      "node_id = excluded.node_id, "
      "description = excluded.description, "
      "followers = excluded.followers, "
      "data_updated_at = CAST(strftime('%s','now') AS INTEGER)",
      org.id,
      this->escape(org.login),
      this->escape(org.node_id),
      this->escape(org.description),
      org.followers);
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
  WRAP_FUNC(this->update_version(org.login, type))
  return EXIT_SUCCESS;
}

int SQLiteDatabase::upsert_org_with_version(std::vector<Org> orgs, enum request_type type) {
  WRAP_FUNC(this->upsert_org(orgs))
  std::vector<std::string> keys;
  keys.reserve(orgs.size());
  for (const auto &org : orgs) {
    keys.push_back(std::to_string(org.id));
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

std::vector<std::string> SQLiteDatabase::list_orgs_random(enum request_type type) {
  return this->list_x_random("orgs", "login", type);
}

int64_t SQLiteDatabase::count_org() {
  return this->count_x("orgs");
}

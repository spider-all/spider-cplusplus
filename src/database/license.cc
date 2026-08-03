#include <database/sqlite.h>

int SQLiteDatabase::upsert_license(License license) {
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO licenses (key, name, spdx_id, node_id, description, implementation, "
      "permissions, conditions, limitations, body, featured) "
      "VALUES ('{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', {})",
      this->escape(license.key),
      this->escape(license.name),
      this->escape(license.spdx_id),
      this->escape(license.node_id),
      this->escape(license.description),
      this->escape(license.implementation),
      this->escape(license.permissions),
      this->escape(license.conditions),
      this->escape(license.limitations),
      this->escape(license.body),
      license.featured ? "true" : "false");
  return this->execute(sql);
}

int SQLiteDatabase::upsert_license_with_version(License license, enum request_type type) {
  WRAP_FUNC(this->upsert_license(license))
  WRAP_FUNC(this->update_version(license.key, type))
  return EXIT_SUCCESS;
}

int64_t SQLiteDatabase::count_license() {
  return this->count_x("licenses");
}
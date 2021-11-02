#include <database/mongo.h>

int Mongo::upsert_license(License license) {
  bsoncxx::document::value doc = make_document(
      kvp("key", license.key),
      kvp("name", license.name),
      kvp("spdx_id", license.spdx_id),
      kvp("node_id", license.node_id),
      kvp("description", license.description),
      kvp("implementation", license.implementation),
      kvp("permissions", license.permissions),
      kvp("conditions", license.conditions),
      kvp("limitations", license.limitations),
      kvp("body", license.body),
      kvp("featured", license.featured));
  bsoncxx::document::value filter = make_document(kvp("key", license.key));
  return this->upsert_x("licenses", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_license_with_version(License license, enum request_type type) {
  WRAP_FUNC(this->upsert_license(license))
  WRAP_FUNC(this->update_version(license.key, type))
  return EXIT_SUCCESS;
}

int64_t Mongo::count_license() {
  return this->count_x("licenses");
}

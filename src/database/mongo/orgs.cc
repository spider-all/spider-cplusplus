#include <database/mongo.h>

int Mongo::upsert_org(Org org) {
  bsoncxx::document::value doc = make_document(
      kvp("id", org.id),
      kvp("login", org.login),
      kvp("node_id", org.node_id),
      kvp("description", org.description));
  bsoncxx::document::value filter = make_document(kvp("id", org.id));
  return this->upsert_x("orgs", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_org(std::vector<Org> orgs) {
  std::map<std::string, std::string> filters;
  for (auto org : orgs) {
    bsoncxx::document::value record = make_document(
        kvp("id", org.id),
        kvp("login", org.login),
        kvp("node_id", org.node_id),
        kvp("description", org.description));
    bsoncxx::document::value filter = make_document(kvp("id", org.id));
    filters.insert(std::pair(bsoncxx::to_json(filter), bsoncxx::to_json(record)));
  }
  return this->upsert_x("orgs", filters);
}

int Mongo::upsert_org_with_version(Org org, enum request_type type) {
  WRAP_FUNC(this->upsert_org(org))
  WRAP_FUNC(this->update_version(org.login, type))
  return EXIT_SUCCESS;
}

int Mongo::upsert_org_with_version(std::vector<Org> orgs, enum request_type type) {
  WRAP_FUNC(this->upsert_org(orgs))
  std::vector<std::string> keys;
  keys.reserve(orgs.size());
  for (const auto &org : orgs) {
    keys.push_back(std::to_string(org.id));
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

std::vector<std::string> Mongo::list_orgs_random(enum request_type type) {
  return this->list_x_random("orgs", "login", type);
}

int64_t Mongo::count_org() {
  return this->count_x("orgs");
}

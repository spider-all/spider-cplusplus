#include <database/mongo.h>

int Mongo::upsert_org(Org org) {
  bsoncxx::document::value doc = make_document(
      kvp("id", org.id),
      kvp("login", org.login),
      kvp("node_id", org.node_id),
      kvp("description", org.description));
  bsoncxx::document::value filter = make_document(kvp("id", org.id));
  return this->upsert_x("orgs", filter.view(), doc.view());
}

int Mongo::upsert_org_with_version(Org org, enum request_type type) {
  int code = this->upsert_org(org);
  if (code != 0) {
    return code;
  }
  code = this->update_version(org.login, type);
  if (code != 0) {
    return code;
  }
  return EXIT_SUCCESS;
}

std::vector<std::string> Mongo::list_orgs_random(enum request_type type) {
  return this->list_x_random("orgs", "login", type);
}

int64_t Mongo::count_org() {
  return this->count_x("orgs");
}

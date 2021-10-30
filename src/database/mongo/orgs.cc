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
  std::string type_string = request_type_string(type);

  std::vector<std::string> orgs;
  try {
    std::string func_name = this->function_name_helper(__func__);
    GET_CONNECTION(this->uri->database(), func_name)

    mongocxx::pipeline stages;
    stages.lookup(make_document(
        kvp("from", fmt::format("{}_version", type_string)),
        kvp("localField", "login"),
        kvp("foreignField", "login"),
        kvp("as", fmt::format("{}_version", type_string))));

    int64_t version = this->versions->get(type);

    bsoncxx::document::view_or_value filter1 = make_document(kvp(fmt::format("{}_version.version", type_string), make_document(kvp("$lt", version))));
    bsoncxx::document::view_or_value filter2 = make_document(kvp(fmt::format("{}_version", type_string), make_document(kvp("$size", 0))));
    stages.match(make_document(kvp("$or", make_array(filter1, filter2))));

    stages.sample(this->sample_size);

    mongocxx::options::aggregate option;
    option.max_time(std::chrono::milliseconds(5000));
    auto cursor = coll.aggregate(stages, option);
    for (auto &&doc : cursor) {
      orgs.emplace_back(doc["login"].get_utf8().value.data());
    }
    if (orgs.empty()) {
      this->incr_version(type);
    } else {
      this->update_version(orgs, type);
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return orgs;
}

int64_t Mongo::count_org() {
  return this->count_x("orgs");
}

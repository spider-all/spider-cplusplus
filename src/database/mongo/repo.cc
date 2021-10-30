#include <database/mongo.h>

int Mongo::create_repo(Repo repo, enum request_type type) {
  try {
    GET_CONNECTION(this->uri->database(), "repos")
    bsoncxx::document::value doc_value = make_document(
        kvp("id", repo.id),
        kvp("node_id", repo.node_id),
        kvp("name", repo.name),
        kvp("full_name", repo.full_name),
        kvp("xprivate", repo.xprivate),
        kvp("owner", repo.owner),
        kvp("owner_type", repo.owner_type),
        kvp("fork", repo.fork),
        kvp("created_at", repo.created_at),
        kvp("updated_at", repo.updated_at),
        kvp("pushed_at", repo.pushed_at),
        kvp("homepage", repo.homepage),
        kvp("size", repo.size),
        kvp("stargazers_count", repo.stargazers_count),
        kvp("watchers_count", repo.watchers_count),
        kvp("forks_count", repo.forks_count),
        kvp("language", repo.language),
        kvp("license", repo.license),
        kvp("forks", repo.forks),
        kvp("open_issues", repo.open_issues),
        kvp("watchers", repo.watchers),
        kvp("default_branch", repo.default_branch));
    mongocxx::options::update option;
    option.upsert(true);
    coll.update_one(make_document(kvp("id", repo.id)), make_document(kvp("$set", doc_value)), option);

    {
      auto coll = database[fmt::format("{}_version", request_type_string(type))];
      mongocxx::options::update option;
      option.upsert(true);
      int64_t version = this->versions->get(type);
      coll.update_one(make_document(kvp("id", repo.id)),
                      make_document(
                          kvp("$set",
                              make_document(kvp("id", repo.id),
                                            kvp("version", version)))),
                      option);
    }

  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int64_t Mongo::count_repo() {
  return this->count_x("repos");
}

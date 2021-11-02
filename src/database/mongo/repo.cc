#include <database/mongo.h>

int Mongo::upsert_repo(Repo repo) {
  bsoncxx::document::value doc = make_document(
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
  bsoncxx::document::value filter = make_document(kvp("id", repo.id));
  return this->upsert_x("repos", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_repo(std::vector<Repo> repos) {
  std::map<std::string, std::string> filters;
  for (auto repo : repos) {
    bsoncxx::document::value record = make_document(
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
    bsoncxx::document::value filter = make_document(kvp("id", repo.id));
    filters.insert(std::pair(bsoncxx::to_json(filter), bsoncxx::to_json(record)));
  }
  return this->upsert_x("repos", filters);
}

int Mongo::upsert_repo_with_version(Repo repo, enum request_type type) {
  WRAP_FUNC(this->upsert_repo(repo))
  WRAP_FUNC(this->update_version(std::to_string(repo.id), type))
  return EXIT_SUCCESS;
}

int Mongo::upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) {
  WRAP_FUNC(this->upsert_repo(repos))
  std::vector<std::string> keys;
  for (auto repo : repos) {
    keys.push_back(std::to_string(repo.id));
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

std::vector<std::string> Mongo::list_repos_random(enum request_type type) {
  return this->list_x_random("repos", "name:owner", type);
}

int64_t Mongo::count_repo() {
  return this->count_x("repos");
}

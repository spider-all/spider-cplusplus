#include <database/mongo.h>

bsoncxx::document::value Mongo::make_commit(Commit commit) {
  return make_document(
      kvp("owner", commit.owner),
      kvp("repo", commit.repo),
      kvp("branch", commit.branch),
      kvp("sha", commit.sha),
      kvp("node_id", commit.node_id),
      kvp("commit", commit.commit),
      kvp("committer", commit.committer),
      kvp("author", commit.author),
      kvp("message", commit.message),
      kvp("url", commit.url),
      kvp("comment_count", commit.comment_count),
      kvp("parents", commit.parents));
}

int Mongo::upsert_commit(Commit commit) {
  bsoncxx::document::value doc = this->make_commit(commit);
  bsoncxx::document::value filter = make_document(kvp("name", commit.owner), kvp("repo", commit.repo), kvp("branch", commit.branch));
  return this->upsert_x("commits", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_commit(std::vector<Commit> commits) {
  std::map<std::string, std::string> filters;
  for (auto commit : commits) {
    bsoncxx::document::value record = this->make_commit(commit);
    bsoncxx::document::value filter = make_document(kvp("name", commit.owner), kvp("repo", commit.repo), kvp("branch", commit.branch));
    filters.insert(std::pair(bsoncxx::to_json(filter), bsoncxx::to_json(record)));
  }
  return this->upsert_x("commits", filters);
}

int Mongo::upsert_commit_with_version(Commit commit, enum request_type type) {
  WRAP_FUNC(this->upsert_commit(commit))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}:{}", commit.owner, commit.repo, commit.branch), type))
  return EXIT_SUCCESS;
}

int Mongo::upsert_commit_with_version(std::vector<Commit> commits, enum request_type type) {
  WRAP_FUNC(this->upsert_commit(commits))
  std::vector<std::string> keys;
  for (const auto &commit : commits) {
    keys.push_back(fmt::format("{}:{}:{}", commit.owner, commit.repo, commit.branch));
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

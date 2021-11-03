#include <database/mongo.h>

int Mongo::upsert_branch(Branch branch) {
  bsoncxx::document::value doc = make_document(
      kvp("repo", branch.repo),
      kvp("name", branch.name),
      kvp("commit", branch.commit));
  bsoncxx::document::value filter = make_document(kvp("repo", branch.repo));
  return this->upsert_x("branches", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_branch(std::vector<Branch> branches) {
  std::map<std::string, std::string> filters;
  for (auto branch : branches) {
    bsoncxx::document::value record = make_document(
        kvp("repo", branch.repo),
        kvp("name", branch.name),
        kvp("commit", branch.commit));
    bsoncxx::document::value filter = make_document(kvp("repo", branch.repo));
    filters.insert(std::pair(bsoncxx::to_json(filter), bsoncxx::to_json(record)));
  }
  return this->upsert_x("branches", filters);
}

int Mongo::upsert_branch_with_version(Branch branch, enum request_type type) {
  WRAP_FUNC(this->upsert_branch(branch))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}", branch.repo, branch.name), type))
  return EXIT_SUCCESS;
}

int Mongo::upsert_branch_with_version(std::vector<Branch> branches, enum request_type type) {
  WRAP_FUNC(this->upsert_branch(branches))
  std::vector<std::string> keys;
  for (const auto &branch : branches) {
    keys.push_back(branch.name);
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

#include <database/mongo.h>

bsoncxx::document::value Mongo::make_branch(Branch branch) {
  return make_document(
      kvp("repo", branch.repo),
      kvp("name", branch.name),
      kvp("commit", branch.commit));
}

int Mongo::upsert_branch(Branch branch) {
  bsoncxx::document::value doc = this->make_branch(branch);
  bsoncxx::document::value filter = make_document(kvp("repo", branch.repo));
  return this->upsert_x("branches", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_branch(std::vector<Branch> branches) {
  std::map<std::string, std::string> filters;
  for (auto branch : branches) {
    bsoncxx::document::value record = this->make_branch(branch);
    bsoncxx::document::value filter = make_document(kvp("repo", branch.repo));
    filters.insert(std::pair(bsoncxx::to_json(filter), bsoncxx::to_json(record)));
  }
  return this->upsert_x("branches", filters);
}

int Mongo::upsert_branch_with_version(Branch branch, enum request_type type) {
  WRAP_FUNC(this->upsert_branch(branch))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}", branch.owner, branch.repo), type))
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

std::vector<std::string> Mongo::list_branches_random(enum request_type type) {
  return this->list_x_random("branches", "repo:name:commit", type);
}

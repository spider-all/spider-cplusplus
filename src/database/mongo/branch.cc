#include <database/mongo.h>

int Mongo::upsert_branch(Branch branch) {
  bsoncxx::document::value doc = make_document(
      kvp("repo", branch.repo),
      kvp("name", branch.name),
      kvp("commit", branch.commit));
  bsoncxx::document::value filter = make_document(kvp("repo", branch.repo));
  return this->upsert_x("orgs", filter.view(), doc.view());
}

int Mongo::upsert_branch_with_version(Branch branch, enum request_type type) {
  WRAP_FUNC(this->upsert_branch(branch))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}", branch.repo, branch.name), type))
  return EXIT_SUCCESS;
}

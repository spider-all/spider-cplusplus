#include <database/duckdb.h>

int DuckDBDatabase::upsert_branch(Branch branch) {
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO branches (owner, repo, name, commit) VALUES ('{}', '{}', '{}', '{}')",
      this->escape(branch.owner),
      this->escape(branch.repo),
      this->escape(branch.name),
      this->escape(branch.commit));
  return this->execute(sql);
}

int DuckDBDatabase::upsert_branch(std::vector<Branch> branches) {
  for (const auto &branch : branches) {
    WRAP_FUNC(this->upsert_branch(branch))
    spdlog::info("Upsert branch: {}", branch.name);
  }
  return EXIT_SUCCESS;
}

int DuckDBDatabase::upsert_branch_with_version(Branch branch, enum request_type type) {
  WRAP_FUNC(this->upsert_branch(branch))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}", branch.owner, branch.repo), type))
  return EXIT_SUCCESS;
}

int DuckDBDatabase::upsert_branch_with_version(std::vector<Branch> branches, enum request_type type) {
  WRAP_FUNC(this->upsert_branch(branches))
  std::vector<std::string> keys;
  for (const auto &branch : branches) {
    keys.push_back(branch.name);
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

std::vector<std::string> DuckDBDatabase::list_branches_random(enum request_type type) {
  return this->list_x_random("branches", "owner;repo;name;commit", type);
}

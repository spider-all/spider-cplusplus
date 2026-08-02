#include <database/duckdb.h>

int DuckDBDatabase::upsert_repo(Repo repo) {
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO repos (id, node_id, name, full_name, xprivate, owner, owner_type, "
      "description, fork, created_at, updated_at, pushed_at, homepage, size, "
      "stargazers_count, watchers_count, forks_count, language, license, forks, "
      "open_issues, watchers, default_branch) "
      "VALUES ({}, '{}', '{}', '{}', {}, '{}', '{}', '{}', {}, '{}', '{}', '{}', '{}', "
      "{}, {}, {}, {}, '{}', '{}', {}, {}, {}, '{}')",
      repo.id,
      this->escape(repo.node_id),
      this->escape(repo.name),
      this->escape(repo.full_name),
      repo.xprivate ? "true" : "false",
      this->escape(repo.owner),
      this->escape(repo.owner_type),
      this->escape(repo.description),
      repo.fork ? "true" : "false",
      this->escape(repo.created_at),
      this->escape(repo.updated_at),
      this->escape(repo.pushed_at),
      this->escape(repo.homepage),
      repo.size,
      repo.stargazers_count,
      repo.watchers_count,
      repo.forks_count,
      this->escape(repo.language),
      this->escape(repo.license),
      repo.forks,
      repo.open_issues,
      repo.watchers,
      this->escape(repo.default_branch));
  return this->execute(sql);
}

int DuckDBDatabase::upsert_repo(std::vector<Repo> repos) {
  for (const auto &repo : repos) {
    WRAP_FUNC(this->upsert_repo(repo))
  }
  return EXIT_SUCCESS;
}

int DuckDBDatabase::upsert_repo_with_version(Repo repo, enum request_type type) {
  WRAP_FUNC(this->upsert_repo(repo))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}", repo.name, repo.owner), type))
  return EXIT_SUCCESS;
}

int DuckDBDatabase::upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) {
  WRAP_FUNC(this->upsert_repo(repos))
  std::vector<std::string> keys;
  for (const auto &repo : repos) {
    keys.push_back(fmt::format("{}:{}", repo.name, repo.owner));
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}

std::vector<std::string> DuckDBDatabase::list_repos_random(enum request_type type) {
  return this->list_x_random("repos", "name;owner", type);
}

int64_t DuckDBDatabase::count_repo() {
  return this->count_x("repos");
}
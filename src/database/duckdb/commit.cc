#include <database/duckdb.h>

int DuckDBDatabase::upsert_commit(Commit commit) {
  std::string sql = fmt::format(
      "INSERT OR REPLACE INTO commits (owner, repo, branch, sha, node_id, commit, committer, "
      "author, message, url, comment_count, parents) "
      "VALUES ('{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}', '{}')",
      this->escape(commit.owner),
      this->escape(commit.repo),
      this->escape(commit.branch),
      this->escape(commit.sha),
      this->escape(commit.node_id),
      this->escape(commit.commit),
      this->escape(commit.committer),
      this->escape(commit.author),
      this->escape(commit.message),
      this->escape(commit.url),
      this->escape(commit.comment_count),
      this->escape(commit.parents));
  return this->execute(sql);
}

int DuckDBDatabase::upsert_commit(std::vector<Commit> commits) {
  for (const auto &commit : commits) {
    WRAP_FUNC(this->upsert_commit(commit))
  }
  return EXIT_SUCCESS;
}

int DuckDBDatabase::upsert_commit_with_version(Commit commit, enum request_type type) {
  WRAP_FUNC(this->upsert_commit(commit))
  WRAP_FUNC(this->update_version(fmt::format("{}:{}:{}", commit.owner, commit.repo, commit.branch), type))
  return EXIT_SUCCESS;
}

int DuckDBDatabase::upsert_commit_with_version(std::vector<Commit> commits, enum request_type type) {
  WRAP_FUNC(this->upsert_commit(commits))
  std::vector<std::string> keys;
  for (const auto &commit : commits) {
    keys.push_back(fmt::format("{}:{}:{}", commit.owner, commit.repo, commit.branch));
  }
  WRAP_FUNC(this->update_version(keys, type))
  return EXIT_SUCCESS;
}
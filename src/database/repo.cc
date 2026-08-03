#include <database/sqlite.h>

int SQLiteDatabase::upsert_repo(Repo repo) {
  std::string sql = fmt::format(
      "INSERT INTO repos (id, node_id, name, full_name, xprivate, owner, owner_type, "
      "description, fork, created_at, updated_at, pushed_at, homepage, size, "
      "stargazers_count, watchers_count, forks_count, language, license, forks, "
      "open_issues, watchers, default_branch, data_created_at, data_updated_at, data_version) "
      "VALUES ({}, '{}', '{}', '{}', {}, '{}', '{}', '{}', {}, '{}', '{}', '{}', '{}', "
      "{}, {}, {}, {}, '{}', '{}', {}, {}, {}, '{}', "
      "CAST(strftime('%s','now') AS INTEGER), CAST(strftime('%s','now') AS INTEGER), 1) "
      "ON CONFLICT(id) DO UPDATE SET "
      "node_id = excluded.node_id, "
      "name = excluded.name, "
      "full_name = excluded.full_name, "
      "xprivate = excluded.xprivate, "
      "owner = excluded.owner, "
      "owner_type = excluded.owner_type, "
      "description = excluded.description, "
      "fork = excluded.fork, "
      "created_at = excluded.created_at, "
      "updated_at = excluded.updated_at, "
      "pushed_at = excluded.pushed_at, "
      "homepage = excluded.homepage, "
      "size = excluded.size, "
      "stargazers_count = excluded.stargazers_count, "
      "watchers_count = excluded.watchers_count, "
      "forks_count = excluded.forks_count, "
      "language = excluded.language, "
      "license = excluded.license, "
      "forks = excluded.forks, "
      "open_issues = excluded.open_issues, "
      "watchers = excluded.watchers, "
      "default_branch = excluded.default_branch, "
      "data_updated_at = CAST(strftime('%s','now') AS INTEGER), "
      "data_version = COALESCE(repos.data_version, 0) + 1",
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

int SQLiteDatabase::upsert_repo(std::vector<Repo> repos) {
  for (const auto &repo : repos) {
    WRAP_FUNC(this->upsert_repo(repo))
  }
  return EXIT_SUCCESS;
}

int SQLiteDatabase::upsert_repo_with_version(Repo repo, enum request_type type) {
  WRAP_FUNC(this->upsert_repo(repo))
  return EXIT_SUCCESS;
}

int SQLiteDatabase::upsert_repo_with_version(std::vector<Repo> repos, enum request_type type) {
  WRAP_FUNC(this->upsert_repo(repos))
  return EXIT_SUCCESS;
}

std::vector<std::string> SQLiteDatabase::list_repos_random(enum request_type type) {
  return this->list_x_random("repos", "id;name;owner", type);
}

int64_t SQLiteDatabase::count_repo() {
  return this->count_x("repos");
}

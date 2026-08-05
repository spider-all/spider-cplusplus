#!/bin/bash
set -euo pipefail

usage() {
  /bin/cat <<'EOF'
Usage:
  scripts/merge_sqlite.sh [--dry-run] [--backup] <source.sqlite> <target.sqlite>

Merge records from source.sqlite into target.sqlite.

Options:
  --dry-run  Print source/target row counts without writing to target.sqlite.
  --backup   Copy target.sqlite to target.sqlite.backup.<timestamp> before merging.
  -h, --help Show this help.
EOF
}

dry_run=0
backup=0
args=()

while (($#)); do
  case "$1" in
  --dry-run)
    dry_run=1
    ;;
  --backup)
    backup=1
    ;;
  -h | --help)
    usage
    exit 0
    ;;
  --)
    shift
    while (($#)); do
      args+=("$1")
      shift
    done
    break
    ;;
  -*)
    echo "unknown option: $1" >&2
    usage >&2
    exit 2
    ;;
  *)
    args+=("$1")
    ;;
  esac
  shift
done

if ((${#args[@]} != 2)); then
  usage >&2
  exit 2
fi

source_db="${args[0]}"
target_db="${args[1]}"

if [[ ! -f "$source_db" ]]; then
  echo "source database does not exist: $source_db" >&2
  exit 1
fi

if [[ ! -f "$target_db" ]]; then
  echo "target database does not exist: $target_db" >&2
  exit 1
fi

source_abs="$(cd "$(/usr/bin/dirname "$source_db")" && pwd -P)/$(/usr/bin/basename "$source_db")"
target_abs="$(cd "$(/usr/bin/dirname "$target_db")" && pwd -P)/$(/usr/bin/basename "$target_db")"

if [[ "$source_abs" == "$target_abs" ]]; then
  echo "source and target database must be different files" >&2
  exit 1
fi

if command -v sqlite3 >/dev/null 2>&1; then
  sqlite_bin="$(command -v sqlite3)"
elif [[ -x /usr/bin/sqlite3 ]]; then
  sqlite_bin="/usr/bin/sqlite3"
else
  echo "sqlite3 is required but was not found" >&2
  exit 1
fi

sql_quote() {
  local value=$1
  value=${value//\'/\'\'}
  printf "'%s'" "$value"
}

source_sql=$(sql_quote "$source_abs")
required_tables=(users orgs repos user_following repo_star)

table_exists() {
  local db=$1
  local table=$2
  "$sqlite_bin" "$db" "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = '$table';"
}

for table in "${required_tables[@]}"; do
  if [[ -z "$(table_exists "$source_abs" "$table")" ]]; then
    echo "source database is missing table: $table" >&2
    exit 1
  fi
  if [[ -z "$(table_exists "$target_abs" "$table")" ]]; then
    echo "target database is missing table: $table" >&2
    exit 1
  fi
done

if ((dry_run)); then
  "$sqlite_bin" "$target_abs" <<SQL
.headers on
.mode column
ATTACH DATABASE $source_sql AS source_db;
SELECT 'users' AS table_name, (SELECT COUNT(*) FROM source_db.users) AS source_rows, (SELECT COUNT(*) FROM main.users) AS target_rows
UNION ALL
SELECT 'orgs', (SELECT COUNT(*) FROM source_db.orgs), (SELECT COUNT(*) FROM main.orgs)
UNION ALL
SELECT 'repos', (SELECT COUNT(*) FROM source_db.repos), (SELECT COUNT(*) FROM main.repos)
UNION ALL
SELECT 'user_following', (SELECT COUNT(*) FROM source_db.user_following), (SELECT COUNT(*) FROM main.user_following)
UNION ALL
SELECT 'repo_star', (SELECT COUNT(*) FROM source_db.repo_star), (SELECT COUNT(*) FROM main.repo_star);
SQL
  exit 0
fi

if ((backup)); then
  timestamp="$(/bin/date +%Y%m%d%H%M%S)"
  backup_path="${target_abs}.backup.${timestamp}"
  /bin/cp "$target_abs" "$backup_path"
  echo "backup created: $backup_path"
fi

"$sqlite_bin" "$target_abs" <<SQL
.bail on
.timeout 5000
PRAGMA foreign_keys = OFF;
ATTACH DATABASE $source_sql AS source_db;

BEGIN IMMEDIATE;

INSERT INTO main.users (
  id, login, node_id, type, name, company, blog, location,
  email, hireable, bio, created_at, updated_at, public_gists, public_repos,
  following, followers, data_created_at, data_updated_at, data_version
)
SELECT
  id, login, node_id, type, name, company, blog, location,
  email, hireable, bio, created_at, updated_at, public_gists, public_repos,
  following, followers, data_created_at, data_updated_at, data_version
FROM source_db.users
WHERE true
ON CONFLICT(id) DO UPDATE SET
  login = excluded.login,
  node_id = excluded.node_id,
  type = excluded.type,
  name = excluded.name,
  company = excluded.company,
  blog = excluded.blog,
  location = excluded.location,
  email = excluded.email,
  hireable = excluded.hireable,
  bio = excluded.bio,
  created_at = excluded.created_at,
  updated_at = excluded.updated_at,
  public_gists = excluded.public_gists,
  public_repos = excluded.public_repos,
  following = excluded.following,
  followers = excluded.followers,
  data_created_at = MIN(main.users.data_created_at, excluded.data_created_at),
  data_updated_at = MAX(main.users.data_updated_at, excluded.data_updated_at),
  data_version = MAX(COALESCE(main.users.data_version, 0), COALESCE(excluded.data_version, 0))
WHERE COALESCE(excluded.data_updated_at, 0) >= COALESCE(main.users.data_updated_at, 0);

INSERT INTO main.orgs (
  id, login, node_id, description, followers,
  data_created_at, data_updated_at, data_version
)
SELECT
  id, login, node_id, description, followers,
  data_created_at, data_updated_at, data_version
FROM source_db.orgs
WHERE true
ON CONFLICT(id) DO UPDATE SET
  login = excluded.login,
  node_id = excluded.node_id,
  description = excluded.description,
  followers = excluded.followers,
  data_created_at = MIN(main.orgs.data_created_at, excluded.data_created_at),
  data_updated_at = MAX(main.orgs.data_updated_at, excluded.data_updated_at),
  data_version = MAX(COALESCE(main.orgs.data_version, 0), COALESCE(excluded.data_version, 0))
WHERE COALESCE(excluded.data_updated_at, 0) >= COALESCE(main.orgs.data_updated_at, 0);

INSERT INTO main.repos (
  id, node_id, name, full_name, xprivate, owner, owner_type,
  description, fork, created_at, updated_at, pushed_at, homepage, size,
  stargazers_count, watchers_count, forks_count, language, license, forks,
  open_issues, watchers, default_branch,
  data_created_at, data_updated_at, data_version
)
SELECT
  id, node_id, name, full_name, xprivate, owner, owner_type,
  description, fork, created_at, updated_at, pushed_at, homepage, size,
  stargazers_count, watchers_count, forks_count, language, license, forks,
  open_issues, watchers, default_branch,
  data_created_at, data_updated_at, data_version
FROM source_db.repos
WHERE true
ON CONFLICT(id) DO UPDATE SET
  node_id = excluded.node_id,
  name = excluded.name,
  full_name = excluded.full_name,
  xprivate = excluded.xprivate,
  owner = excluded.owner,
  owner_type = excluded.owner_type,
  description = excluded.description,
  fork = excluded.fork,
  created_at = excluded.created_at,
  updated_at = excluded.updated_at,
  pushed_at = excluded.pushed_at,
  homepage = excluded.homepage,
  size = excluded.size,
  stargazers_count = excluded.stargazers_count,
  watchers_count = excluded.watchers_count,
  forks_count = excluded.forks_count,
  language = excluded.language,
  license = excluded.license,
  forks = excluded.forks,
  open_issues = excluded.open_issues,
  watchers = excluded.watchers,
  default_branch = excluded.default_branch,
  data_created_at = MIN(main.repos.data_created_at, excluded.data_created_at),
  data_updated_at = MAX(main.repos.data_updated_at, excluded.data_updated_at),
  data_version = MAX(COALESCE(main.repos.data_version, 0), COALESCE(excluded.data_version, 0))
WHERE COALESCE(excluded.data_updated_at, 0) >= COALESCE(main.repos.data_updated_at, 0);

INSERT INTO main.user_following (
  upstream_user_id, downstream_user_id,
  data_created_at, data_updated_at, data_version
)
SELECT
  upstream_user_id, downstream_user_id,
  data_created_at, data_updated_at, data_version
FROM source_db.user_following
WHERE true
ON CONFLICT(upstream_user_id, downstream_user_id) DO UPDATE SET
  data_created_at = MIN(main.user_following.data_created_at, excluded.data_created_at),
  data_updated_at = MAX(main.user_following.data_updated_at, excluded.data_updated_at),
  data_version = MAX(COALESCE(main.user_following.data_version, 0), COALESCE(excluded.data_version, 0))
WHERE COALESCE(excluded.data_updated_at, 0) >= COALESCE(main.user_following.data_updated_at, 0);

INSERT INTO main.repo_star (
  user_id, repo_id,
  data_created_at, data_updated_at, data_version
)
SELECT
  user_id, repo_id,
  data_created_at, data_updated_at, data_version
FROM source_db.repo_star
WHERE true
ON CONFLICT(user_id, repo_id) DO UPDATE SET
  data_created_at = MIN(main.repo_star.data_created_at, excluded.data_created_at),
  data_updated_at = MAX(main.repo_star.data_updated_at, excluded.data_updated_at),
  data_version = MAX(COALESCE(main.repo_star.data_version, 0), COALESCE(excluded.data_version, 0))
WHERE COALESCE(excluded.data_updated_at, 0) >= COALESCE(main.repo_star.data_updated_at, 0);

COMMIT;

DETACH DATABASE source_db;
VACUUM;
SQL

echo "merge completed: $source_abs -> $target_abs"

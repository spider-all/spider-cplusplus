# AGENTS.md - Instructions for AI Agents

## Project Overview

**spider-cplusplus** is a C++20 application that crawls GitHub APIs and stores discovered orgs, repos, commits, branches, emojis, gitignore templates, and licenses into MongoDB. It exposes an HTTP API with Prometheus metrics.

## Tech Stack

- **Language**: C++20
- **Build**: CMake 3.10+ + Make
- **Dependencies** (managed via vcpkg): CLI11, nlohmann-json, yaml-cpp, OpenSSL, mongocxx, libfort, ZLIB, spdlog, fmt, curl, cpp-httplib, prometheus-cpp, boost-algorithm, boost-random, gtest, croncpp, utf8proc
- **Database**: MongoDB (via mongocxx driver)
- **Metrics**: Prometheus (prometheus-cpp pull mode)
- **Deployment**: Docker multi-arch (linux/amd64, linux/arm64) + Helm charts
- **CI/CD**: GitHub Actions (build, PR, deps)

## Project Structure

```text
spider-cplusplus/
├── src/                          # Source files
│   ├── spider.cc                 # Entry point (CLI parsing, signal handling)
│   ├── config.cc                 # YAML config parsing
│   ├── versions.cc               # Version tracking
│   ├── application/
│   │   ├── server.cc             # HTTP server (cpp-httplib)
│   │   ├── prome.cc              # Prometheus metrics
│   │   └── request/              # GitHub API request handlers
│   │       ├── request.cc        # Base request logic
│   │       ├── orgs.cc           # Organization crawler
│   │       ├── license.cc        # License crawler
│   │       ├── xrepos.cc         # Repository crawler
│   │       ├── info.cc           # User info crawler
│   │       ├── followx.cc        # Followers/Followings crawler
│   │       ├── branch.cc         # Branch crawler
│   │       ├── commit.cc         # Commit crawler
│   │       ├── emojis.cc         # Emoji crawler
│   │       └── gitignore.cc      # Gitignore crawler
│   └── database/mongo/           # MongoDB database layer
│       ├── mongo.cc              # MongoDB connection
│       ├── user.cc, orgs.cc, repo.cc, branch.cc, commit.cc, etc.
│       └── collections.cc, common.cc, emoji.cc, gitignore.cc, license.cc
├── include/                      # Header files
│   ├── config.h, const.h, error.h, model.h, versions.h
│   ├── application.h, database.h, common.h
│   ├── application/
│   │   ├── server.h, request.h, prome.h
│   └── database/mongo.h
├── test/                         # Unit tests (gtest)
│   ├── ensure_index.cc
│   ├── list_x_random.cc
│   └── create_x_collection.cc
├── docker/                       # Dockerfiles
│   ├── Dockerfile                # Production image
│   ├── dev.Dockerfile            # Dev image
│   └── base.Dockerfile           # Base image with vcpkg deps
├── manifests/spider-cplusplus/   # Helm charts
├── .github/workflows/            # CI/CD
│   ├── build.yml                 # Push to main: build & push multi-arch image
│   ├── pr.yml                    # PR: build test image
│   └── deps.yml                  # Dependency updates
├── vcpkg.json                    # vcpkg manifest dependencies
├── config.yaml.sample            # Sample config
├── CMakeLists.txt                # CMake build config
├── Makefile                      # Build targets: build, release, debug, deps, image, changelog, clean
└── VERSION                       # Current version
```

## Build & Run

```bash
# Install dependencies via vcpkg (manifest mode, requires VCPKG_ROOT)
make deps

# Debug build (requires VCPKG_ROOT set)
make debug

# Release build (requires VCPKG_ROOT set)
make release

# Clean build output
make clean

# Build Docker image
make image

# Generate changelog
make changelog
```

The CMake build uses C++20 standard and links against static libraries for mongocxx/bsoncxx. The vcpkg toolchain is loaded from `$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake`.

## Code Style

- **C++ Standard**: C++20
- **Formatting**: `.clang-format` at project root (run clang-format before committing)
- **Headers**: Use `#pragma once` for header guards
- **Error Handling**: Return integer error codes defined in `error.h`:
  - `SPIDER_OK = 0`
  - `UNKNOWN_REQUEST_TYPE = -1`
  - `REQUEST_ERROR = -2`
  - `DATABASE_OPEN_ERROR = -3`
  - `CONFIG_PARSE_ERROR = -4`
  - `SQL_EXEC_ERROR = -5`
  - `DATABASE_SQL_ERROR = -6`
- **Error Propagation**: Use `WRAP_FUNC(func)` macro from `common.h` for error propagation
- **Logging**: Use `spdlog` (spdlog::info, spdlog::error, etc.)
- **JSON**: Use `nlohmann::json` with `to_json`/`from_json` ADL overloads
- **Config**: YAML-based configuration via yaml-cpp
- **Naming**: snake_case for variables/functions, PascalCase for classes
- **Database Interface**: Abstract `Database` class with virtual methods; concrete `Mongo` implementation

## Architecture

1. **Entry Point** (`spider.cc`): CLI parsing via CLI11, signal handling (SIGINT/SIGTERM), initializes database and application
2. **Config** (`config.h`): YAML config with fields for database DSN, crawler tokens, user-agent, timezone, sleep interval, and crawler type toggles
3. **Database Interface** (`database.h`): Abstract class with CRUD operations for users, orgs, repos, branches, commits, emojis, gitignore, licenses + version tracking
4. **Application** (`application.h`): Abstract `Application` class
5. **Server** (`server.h`): cpp-httplib based HTTP server with pagination support (page/limit/query params)
6. **Request Handlers** (`request.h`): GitHub API crawlers, each implementing a specific data type
7. **Prometheus** (`prome.h`): Metrics endpoint

## Configuration

Config file is YAML format. See `config.yaml.sample` for reference:

- `entry`: GitHub username to start crawling from
- `token`: List of GitHub tokens for API authentication
- `useragent`: User-Agent string for HTTP requests
- `timezone`: Timezone for scheduling
- `sleep_each_request`: Delay between requests (ms)
- `crawler.*`: Toggle which data types to crawl
- `database.type`: Database backend (currently only "mongodb")
- `database.mongodb.dsn`: MongoDB connection string

## Testing

Tests are in the `test/` directory using Google Test (gtest). Run with ctest or directly:

```bash
cd build/debug && ctest
```

## CI/CD

- **PRs**: Builds Docker image for linux/amd64 (no push)
- **Push to main**: Builds & pushes multi-arch Docker image (linux/amd64 + linux/arm64) to `ghcr.io/spider-all/spider-cplusplus`
- Images are tagged with datetime stamp

## Key Dependencies

| Library         | Purpose                    |
| --------------- | -------------------------- |
| CLI11           | CLI argument parsing       |
| nlohmann-json   | JSON parsing/serialization |
| yaml-cpp        | YAML config parsing        |
| cpp-httplib     | HTTP server                |
| curl            | HTTP client for GitHub API |
| mongocxx        | MongoDB driver             |
| prometheus-cpp  | Metrics                    |
| spdlog          | Structured logging         |
| gtest           | Unit testing               |
| croncpp         | Cron expression parsing    |
| utf8proc        | UTF-8 string processing    |
| libfort         | Terminal table formatting  |
| boost-algorithm | String algorithms          |
| boost-random    | Random number generation   |

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

## SVG Diagram Rules

Use these rules when creating or editing repository SVG architecture or relationship diagrams:

### Layout and Alignment

- Use a white background.
- Keep the top-level title short and concrete, for example `crawler and tables`.
- Use concise column titles, for example `crawler`, `GitHub API`, and `tables`; do not add subtitle explanations under column titles.
- Keep all container rectangles large enough to fully enclose their child rectangles, with consistent inner padding on all sides.
- Keep the overall drawing balanced inside the SVG viewBox; avoid excessive whitespace on one side and cramped content on the other.
- Keep adjacent container gutters equal whenever possible; use one explicit gutter width consistently across the drawing.
- The current relationship diagram uses `60px` container gutters and `30px` horizontal inner padding.
- Keep child rectangles in the same column left-aligned, with consistent widths and consistent vertical spacing.
- Use one consistent child-rectangle height across columns when practical; the current relationship diagram uses `42px`.
- Align rectangles that have a direct horizontal relationship to the same vertical centerline and, when practical, the same top and bottom edges.
- Preserve a regular row rhythm; do not shift a node vertically unless the routing semantics require a distinct row.
- Keep crawler rectangles concise; include only the crawler name, without extra descriptions.
- Keep GitHub API rectangles concise; use the API path itself as the title, without extra descriptions.
- Prefer one unique table rectangle per table; do not duplicate table rectangles such as `users` or `repos`.

### Connector Routing

- Use horizontal and vertical straight runs only; do not introduce diagonal line segments.
- Use explicit quadratic curves for rounded corners, for example `Q`, rather than relying only on `stroke-linejoin`.
- Use one corner radius consistently; the current relationship diagram uses `10px`.
- Make every rounded curve end exactly at the tangent point of the next straight segment.
- Never start or end another path at a curve control point; control points are not points on the rendered curve.
- Keep crawler-to-GitHub-API lines horizontal whenever possible.
- Keep GitHub-API-to-table lines horizontal whenever the source and target rectangles are aligned.
- When a node has two right-side outputs, place them at one-third and two-thirds of the rectangle height.
- If a crawler fans out to multiple GitHub API rectangles, use a shared vertical split point at the same x-coordinate.
- Place shared split points at the midpoint of the gutter between adjacent container rectangles whenever possible.
- For GitHub API internal chains, prefer short orthogonal routes between adjacent API columns.
- Keep line endpoints visually centered on the relevant rectangle edge unless multiple outputs require one-third and two-thirds positions.
- Distribute multiple vertical trunk routes evenly across the gutter between adjacent containers.
- Draw each shared trunk exactly once; never stack coincident path segments because repeated strokes make the trunk and corners appear thicker.
- Draw fan-out branches as only the unique rounded corner and horizontal segment that leave the shared trunk.
- Use marker-free join paths for branches that merge into another trunk; place the arrow marker only on the final path entering the target.
- For table-to-crawler input lines, route from the right side of the table to the left side of the crawler.
- Keep table-to-crawler bends outside the large column rectangles whenever possible.
- Keep external trunk offsets geometrically consistent around a container; use the same offset sequence on the left, right, and bottom edges.
- The current relationship diagram uses `25px` increments for nested external trunk offsets.
- Keep parallel trunks separated by enough spacing to avoid visual merging, with intentional inside/outside ordering.

### Colors and Strokes

- Use one consistent color per table, and use the same color for the table rectangle, all lines pointing to that table, and all lines feeding from that table.
- Use red lines for crawler-to-GitHub-API request flow.
- Use table-specific colored lines for GitHub-API-to-table writes.
- Use table-specific colored solid lines for table-to-crawler input flow.
- Keep base connectors solid; dashed strokes are reserved for a clearly distinct semantic layer or a transient animation overlay.
- Use a consistent stroke hierarchy so containers, nodes, connectors, and animation overlays remain visually distinct.
- The current relationship diagram uses `1.8px` container and node borders, `2px` primary connectors, `1.8px` feedback connectors, and a `4px` animation overlay.
- Use `stroke-linecap: round` and `stroke-linejoin: round` for connector paths.
- Keep arrowheads smaller than the moving flow band and proportional to the base connector width.
- Give markers an explicit `viewBox` so reducing `markerWidth` and `markerHeight` does not clip the arrowhead.
- The current relationship diagram uses `7x7` arrow markers with `markerUnits="strokeWidth"`.
- Remove legends when colors and labels make the diagram self-explanatory.

### Connector Animation

- Keep the solid connector group as the complete static representation of the diagram.
- Add animation as a second physical group of paths above the solid connectors; do not rely on `<use>` style inheritance because some SVG viewers do not propagate animation properties into referenced paths.
- Keep the animated path data in one-to-one correspondence with the base connector path data.
- Remove arrow markers from the animated overlay so arrowheads are not duplicated.
- Animate `stroke-dashoffset` on the overlay only; do not convert the base connector into a dashed line.
- Make the moving band wider and more visible than the base connector while keeping its opacity low enough that labels and intersections remain readable.
- Keep the dash period and animation offset equal so the loop is seamless.
- The current relationship diagram uses `stroke-dasharray: 10 18`, `stroke-dashoffset: -28`, `opacity: 0.6`, and a `1.2s` linear cycle.
- Move the overlay in the same direction as the arrow flow.
- Add a `prefers-reduced-motion: reduce` rule that disables the animated overlay.
- Ensure the diagram remains complete and readable when animation is disabled or when rendered by a static previewer.

### Validation

- Validate SVG XML after editing.
- Check that every directional change uses a rounded curve and that no unintended diagonal segment remains.
- Check that shared trunks are rendered by a single base path rather than repeated coincident segments.
- Check that every branch starts at the rendered tangent point, not at the curve control point.
- Verify that the base connector count matches the animated overlay path count.
- Verify animation in a browser; Quick Look and other static image previewers may display only one frame.
- Render a static PNG to confirm that the non-animated fallback remains correct.
- Run `git diff --check` for the SVG file after editing.

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

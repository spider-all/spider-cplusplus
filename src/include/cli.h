#include <iostream>
#include <unistd.h>

#include <SQLiteCpp/SQLiteCpp.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <leveldb/db.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#pragma once

#define SPIDER_VERSION_MAJOR 1
#define SPIDER_VERSION_MINOR 2
#define SPIDER_VERSION_PATCH 0

class CommandLine {
public:
  static char *cli(int argc, char *argv[]);
};

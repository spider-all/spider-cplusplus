#include <iostream>
#include <unistd.h>

#pragma once

#define SPIDER_VERSION_MAJOR 1
#define SPIDER_VERSION_MINOR 2
#define SPIDER_VERSION_PATCH 0

class CommandLine {
public:
  static char *cli(int argc, char *argv[]);
};

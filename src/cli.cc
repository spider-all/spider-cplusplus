#include <cli.h>

#define SPIDER_VERSION_MAJOR 1
#define SPIDER_VERSION_MINOR 2
#define SPIDER_VERSION_PATCH 0

void usage() {
  printf("spider %s\n", "usage:");
  printf("\t%s\t%s\n", "-h", "show this help information");
  printf("\t%s\t%s\n", "-c", "set config file path");
  printf("\t%s\t%s\n", "-v", "get version");
}

std::string CommandLine::cli(int argc, char *argv[]) {
  int option_char;
  while ((option_char = getopt(argc, argv, "c:hv")) != -1) {
    switch (option_char) {
    case 'c':
      return std::string(optarg);
    case 'h':
      usage();
      return "";
    case 'v':
      std::cout << "spider v" << std::to_string(SPIDER_VERSION_MAJOR) << "." << std::to_string(SPIDER_VERSION_MINOR) << "." << std::to_string(SPIDER_VERSION_PATCH) << std::endl;
      return "";
    default:
      return "";
    }
  }
  return "";
}

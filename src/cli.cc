#include <cli.h>

#define SPIDER_VERSION_MAJOR 1
#define SPIDER_VERSION_MINOR 2
#define SPIDER_VERSION_PATCH 0

char *CommandLine::cli(int argc, char *argv[]) {
  int option_char;
  while ((option_char = getopt(argc, argv, "c:hv")) != -1) {
    switch (option_char) {
    case 'c':
      return optarg;
    case 'h':
      printf("spider %s\n", "usage:");
      printf("\t%s\t%s\n", "-h", "show this help information");
      printf("\t%s\t%s\n", "-c", "set config file path");
      printf("\t%s\t%s\n", "-v", "get version");
      return nullptr;
    case 'v':
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\n{}", "spider ") << fmt::format(fg(fmt::color::green), "v{}.{}.{}", std::to_string(SPIDER_VERSION_MAJOR), std::to_string(SPIDER_VERSION_MINOR), std::to_string(SPIDER_VERSION_PATCH)) << std::endl;
      std::cout << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "   {:13} ", "SQLiteCpp") << fmt::format(fg(fmt::color::green), "v{}", SQLITECPP_VERSION) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "   {:13} ", "sqlite3") << fmt::format(fg(fmt::color::green), "v{}", SQLITE_VERSION) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "   {:13} ", "yaml-cpp") << fmt::format(fg(fmt::color::green), "v{}", yaml_cpp_version) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "   {:13} ", "cpp-httplib") << fmt::format(fg(fmt::color::green), "v{}", cpp_httplib_version) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "   {:13} ", "leveldb") << fmt::format(fg(fmt::color::green), "v{}.{}", leveldb::kMajorVersion, leveldb::kMinorVersion) << std::endl;
      std::cout << std::endl;
      return nullptr;
    default:
      return nullptr;
    }
  }
  return nullptr;
}

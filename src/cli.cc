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
      std::cout << "\n    Dependencies version:" << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "cpr") << fmt::format(fg(fmt::color::green), "v{}", cpr_version) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "curl") << fmt::format(fg(fmt::color::green), "v{}", LIBCURL_VERSION) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "SQLiteCpp") << fmt::format(fg(fmt::color::green), "v{}", SQLITECPP_VERSION) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "sqlite3") << fmt::format(fg(fmt::color::green), "v{}", SQLITE_VERSION) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "yaml-cpp") << fmt::format(fg(fmt::color::green), "v{}", yaml_cpp_version) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "boost") << fmt::format(fg(fmt::color::green), "v{}.{}.{}", std::to_string(BOOST_VERSION / 100000), std::to_string(BOOST_VERSION / 100 % 1000), std::to_string(BOOST_VERSION % 100)) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "boost/asio") << fmt::format(fg(fmt::color::green), "v{}.{}.{}", std::to_string(BOOST_ASIO_VERSION / 100000), std::to_string(BOOST_ASIO_VERSION / 100 % 1000), std::to_string(BOOST_ASIO_VERSION % 100)) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "boost/beast") << fmt::format(fg(fmt::color::green), "{}", BOOST_BEAST_VERSION_STRING) << std::endl;
      std::cout << fmt::format(fg(fmt::color::golden_rod), "\t{:18} ", "leveldb") << fmt::format(fg(fmt::color::green), "v{}.{}", leveldb::kMajorVersion, leveldb::kMinorVersion) << std::endl;
      std::cout << std::endl;
      return nullptr;
    default:
      return nullptr;
    }
  }
  return nullptr;
}

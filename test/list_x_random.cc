#include <CLI/CLI.hpp>
#include <gtest/gtest.h>

#include <database/duckdb.h>

std::string db_path;

namespace {

class TersePrinter : public testing::EmptyTestEventListener {
private:
  void OnTestProgramStart(const testing::UnitTest & /* unit_test */) override {}

  void OnTestProgramEnd(const testing::UnitTest &unit_test) override {
    fprintf(stdout, "TEST %s\n", unit_test.Passed() ? "PASSED" : "FAILED");
    fflush(stdout);
  }
};

TEST(list_x_random, normal) {
  DuckDBDatabase *ret = new DuckDBDatabase(db_path);
  int code = ret->initialize();
  EXPECT_EQ(code, 0);

  std::vector<std::string> result = ret->list_x_random("users", "login;id:int64", request_type_followers);

  spdlog::info("result size: {}", result.size());
  for (auto &item : result) {
    spdlog::info("item: {}", item);
  }
}
} // namespace

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  CLI::App app{"DuckDB helper test"};
  app.add_option("--db-path", db_path, "duckdb database path");
  CLI11_PARSE(app, argc, argv)

  testing::UnitTest &unit_test = *testing::UnitTest::GetInstance();
  testing::TestEventListeners &listeners = unit_test.listeners();
  delete listeners.Release(listeners.default_result_printer());
  listeners.Append(new TersePrinter);
  return RUN_ALL_TESTS();
}
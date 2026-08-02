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

TEST(create_x_collection, normal) {
  DuckDBDatabase *ret = new DuckDBDatabase(db_path);
  int code = ret->initialize();
  EXPECT_EQ(code, 0);

  code = ret->create_x_collection("tests", "id:int64;name;source");
  EXPECT_EQ(code, 0);
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
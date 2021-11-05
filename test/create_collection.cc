#include <gtest/gtest.h>

#include <database.h>
#include <database/mongo.h>

std::string dsn;

namespace {

class TersePrinter : public testing::EmptyTestEventListener {
private:
  void OnTestProgramStart(const testing::UnitTest & /* unit_test */) override {}

  void OnTestProgramEnd(const testing::UnitTest &unit_test) override {
    fprintf(stdout, "TEST %s\n", unit_test.Passed() ? "PASSED" : "FAILED");
    fflush(stdout);
  }
};

TEST(create_collection, normal) {
  Database *ret = new Mongo(dsn);
  int code = ret->initialize();
  EXPECT_EQ(code, 0);

  auto schema = make_document(kvp("bsonType", "object"), kvp("required", make_array("name")));
  auto doc = make_document(kvp("$jsonSchema", schema));
  code = ret->create_collection("tests", doc.view());
  EXPECT_EQ(code, 0);
}
} // namespace

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  if (argc == 3 && strcmp(argv[1], "--dsn") == 0) {
    dsn = std::string(argv[2]);
  } else {
    printf("%s\n", "params --dsn should be provided");
  }

  testing::UnitTest &unit_test = *testing::UnitTest::GetInstance();
  testing::TestEventListeners &listeners = unit_test.listeners();
  delete listeners.Release(listeners.default_result_printer());
  listeners.Append(new TersePrinter);
  return RUN_ALL_TESTS();
}

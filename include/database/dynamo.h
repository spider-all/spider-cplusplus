#include <cassert>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/CreateTableRequest.h>
#include <aws/dynamodb/model/DescribeTableRequest.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/KeySchemaElement.h>
#include <aws/dynamodb/model/ProvisionedThroughput.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/ScalarAttributeType.h>
#include <aws/dynamodb/model/ScanRequest.h>
#include <spdlog/spdlog.h>

#pragma once

#include <database/database.h>
#include <error.hpp>
#include <model.h>

class Dynamo : public Database {
public:
  std::string database_users = "github_users";
  Aws::SDKOptions options;
  explicit Dynamo(const std::string &);
  ~Dynamo() override;
  int initialize() override;

  int create_user(User) override;
  int64_t count_user() override;
  std::vector<std::string> list_users() override;

  int create_org(Org) override;
  int64_t count_org() override;
  std::vector<std::string> list_orgs() override;
};

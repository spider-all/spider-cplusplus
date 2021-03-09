#include <database/dynamo.h>

DynamoDB::DynamoDB(const std::string &region) {
  Aws::InitAPI(options);
  Aws::Client::ClientConfiguration clientConfig;
  clientConfig.region = region;
  db.dynamo = new Aws::DynamoDB::DynamoDBClient(clientConfig);
}

DynamoDB::~DynamoDB() {
  delete db.dynamo;
  Aws::ShutdownAPI(options);
}

int DynamoDB::initialize() {
  Aws::DynamoDB::Model::DescribeTableRequest dtr;
  dtr.SetTableName(Aws::String(database_users));

  const Aws::DynamoDB::Model::DescribeTableOutcome &result = db.dynamo->DescribeTable(dtr);
  if (result.IsSuccess()) {
    spdlog::info("Table {} exist!", result.GetResult().GetTable().GetTableName());
  } else {
    spdlog::error(result.GetError().GetMessage());

    Aws::DynamoDB::Model::CreateTableRequest req;
    Aws::DynamoDB::Model::AttributeDefinition haskKey;
    haskKey.SetAttributeName("id");
    haskKey.SetAttributeType(Aws::DynamoDB::Model::ScalarAttributeType::S);
    req.AddAttributeDefinitions(haskKey);

    Aws::DynamoDB::Model::KeySchemaElement keyscelt;
    keyscelt.WithAttributeName("id").WithKeyType(Aws::DynamoDB::Model::KeyType::HASH);
    req.AddKeySchema(keyscelt);
    Aws::DynamoDB::Model::ProvisionedThroughput thruput;
    thruput.WithReadCapacityUnits(5).WithWriteCapacityUnits(5);
    req.SetProvisionedThroughput(thruput);

    req.SetTableName(Aws::String(database_users));

    const Aws::DynamoDB::Model::CreateTableOutcome &result = db.dynamo->CreateTable(req);
    if (result.IsSuccess()) {
      spdlog::info("Table {} created!", result.GetError().GetMessage());
    } else {
      spdlog::info("Failed to create table: {}", result.GetError().GetMessage());
    }
  }
  return EXIT_SUCCESS;
}

int DynamoDB::create_user(user) {
  return 0;
}

std::vector<std::string> DynamoDB::list_users() {
  return std::vector<std::string>{};
}

int DynamoDB::count_user() {
  return 0;
}

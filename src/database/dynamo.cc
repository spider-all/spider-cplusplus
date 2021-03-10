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

  const Aws::DynamoDB::Model::DescribeTableOutcome &result =
      db.dynamo->DescribeTable(dtr);
  if (result.IsSuccess()) {
    spdlog::info("Table {} exist!",
                 result.GetResult().GetTable().GetTableName());
  } else {
    Aws::DynamoDB::Model::CreateTableRequest req;

    Aws::DynamoDB::Model::AttributeDefinition primaryKey;
    primaryKey.SetAttributeName("id");
    primaryKey.SetAttributeType(Aws::DynamoDB::Model::ScalarAttributeType::S);
    req.AddAttributeDefinitions(primaryKey);

    Aws::DynamoDB::Model::KeySchemaElement key;
    key.WithAttributeName("id");
    key.WithKeyType(Aws::DynamoDB::Model::KeyType::HASH);
    req.AddKeySchema(key);

    Aws::DynamoDB::Model::ProvisionedThroughput throughput;
    throughput.WithReadCapacityUnits(5);
    throughput.WithWriteCapacityUnits(5);
    req.SetProvisionedThroughput(throughput);

    req.SetTableName(Aws::String(database_users));

    const Aws::DynamoDB::Model::CreateTableOutcome &res =
        db.dynamo->CreateTable(req);
    if (res.IsSuccess()) {
      spdlog::info("Table {} created!",
                   res.GetResult().GetTableDescription().GetTableName());
    } else {
      spdlog::info("Failed to create table: {}", res.GetError().GetMessage());
    }
  }
  return EXIT_SUCCESS;
}

int DynamoDB::create_user(user user) {
  std::string user_id = std::to_string(user.id);

  Aws::DynamoDB::Model::PutItemRequest pir;
  pir.SetTableName(database_users);

  Aws::DynamoDB::Model::AttributeValue user_id_value;
  user_id_value.SetS(user_id);
  pir.AddItem("id", user_id_value);

  Aws::DynamoDB::Model::AttributeValue login_value;
  login_value.SetS(user.login);
  pir.AddItem("login", login_value);

  Aws::DynamoDB::Model::AttributeValue node_id_value;
  node_id_value.SetS(user.node_id);
  pir.AddItem("node_id", node_id_value);

  Aws::DynamoDB::Model::AttributeValue type_value;
  type_value.SetS(user.type);
  pir.AddItem("type", type_value);

  Aws::DynamoDB::Model::AttributeValue name_value;
  name_value.SetS(user.name);
  pir.AddItem("name", name_value);

  Aws::DynamoDB::Model::AttributeValue company_value;
  company_value.SetS(user.company);
  pir.AddItem("company", company_value);

  Aws::DynamoDB::Model::AttributeValue blog_value;
  blog_value.SetS(user.blog);
  pir.AddItem("blog", blog_value);

  Aws::DynamoDB::Model::AttributeValue location_value;
  location_value.SetS(user.location);
  pir.AddItem("location", location_value);

  Aws::DynamoDB::Model::AttributeValue email_value;
  email_value.SetS(user.email);
  pir.AddItem("email", email_value);

  Aws::DynamoDB::Model::AttributeValue hireable_value;
  hireable_value.SetBool(user.hireable);
  pir.AddItem("hireable", hireable_value);

  Aws::DynamoDB::Model::AttributeValue bio_value;
  bio_value.SetS(user.bio);
  pir.AddItem("bio", bio_value);

  Aws::DynamoDB::Model::AttributeValue created_at_value;
  created_at_value.SetS(user.created_at);
  pir.AddItem("created_at", created_at_value);

  Aws::DynamoDB::Model::AttributeValue updated_at_value;
  updated_at_value.SetS(user.updated_at);
  pir.AddItem("updated_at", updated_at_value);

  Aws::DynamoDB::Model::AttributeValue public_gists_value;
  public_gists_value.SetN(user.public_gists);
  pir.AddItem("public_gists", public_gists_value);

  Aws::DynamoDB::Model::AttributeValue public_repos_value;
  public_repos_value.SetN(user.public_repos);
  pir.AddItem("public_repos", public_repos_value);

  Aws::DynamoDB::Model::AttributeValue following_value;
  following_value.SetN(user.following);
  pir.AddItem("following", following_value);

  Aws::DynamoDB::Model::AttributeValue followers_value;
  followers_value.SetN(user.followers);
  pir.AddItem("followers", followers_value);

  const Aws::DynamoDB::Model::PutItemOutcome result = db.dynamo->PutItem(pir);
  if (!result.IsSuccess()) {
    spdlog::error(result.GetError().GetMessage());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

std::vector<std::string> DynamoDB::list_users() {
  std::vector<std::string> users{};

  Aws::DynamoDB::Model::ScanRequest req;
  req.SetTableName(database_users);

  const Aws::DynamoDB::Model::ScanOutcome &result = db.dynamo->Scan(req);
  if (result.IsSuccess()) {
    const Aws::Vector<
        Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>> &items =
        result.GetResult().GetItems();
    if (!items.empty()) {
      for (const auto &item : items) {
        for (const auto &i : item) {
          if (i.first == "login") {
            users.push_back(i.second.GetS());
          }
        }
      }
    }
  } else {
    spdlog::error(result.GetError().GetMessage());
  }

  std::shuffle(
      users.begin(), users.end(),
      std::default_random_engine(
          std::chrono::system_clock::now().time_since_epoch().count()));

  if (users.size() > 100) {
    users.resize(100);
  }

  return users;
}

int DynamoDB::count_user() {
  int count = 0;
  Aws::DynamoDB::Model::ScanRequest req;
  req.SetTableName(database_users);
  const Aws::DynamoDB::Model::ScanOutcome &result = db.dynamo->Scan(req);
  if (result.IsSuccess()) {
    const Aws::Vector<
        Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>>
        items = result.GetResult().GetItems();
    count = items.size();
  } else {
    spdlog::error(result.GetError().GetMessage());
  }
  return count;
}

#include <database/dynamo.h>

using namespace Aws::DynamoDB::Model;

Dynamo::Dynamo(const std::string &region) {
  Aws::InitAPI(options);
  Aws::Client::ClientConfiguration clientConfig;
  clientConfig.region = region;
  db.dynamo = new Aws::DynamoDB::DynamoDBClient(clientConfig);
}

Dynamo::~Dynamo() {
  delete db.dynamo;
  Aws::ShutdownAPI(options);
}

int Dynamo::initialize() {
  DescribeTableRequest dtr;
  dtr.SetTableName(Aws::String(database_users));

  const DescribeTableOutcome &result = db.dynamo->DescribeTable(dtr);
  if (result.IsSuccess()) {
    spdlog::info("Table {} exist!", result.GetResult().GetTable().GetTableName());
  } else {
    CreateTableRequest req;

    AttributeDefinition primaryKey;
    primaryKey.SetAttributeName("id");
    primaryKey.SetAttributeType(ScalarAttributeType::S);
    req.AddAttributeDefinitions(primaryKey);

    KeySchemaElement key;
    key.WithAttributeName("id");
    key.WithKeyType(KeyType::HASH);
    req.AddKeySchema(key);

    ProvisionedThroughput throughput;
    throughput.WithReadCapacityUnits(5);
    throughput.WithWriteCapacityUnits(5);
    req.SetProvisionedThroughput(throughput);

    req.SetTableName(Aws::String(database_users));

    const CreateTableOutcome &res = db.dynamo->CreateTable(req);
    if (!res.IsSuccess()) {
      spdlog::info("Failed to create table: {}", res.GetError().GetMessage());
    }
  }
  return EXIT_SUCCESS;
}

int Dynamo::create_user(User user) {
  std::string user_id = std::to_string(user.id);

  PutItemRequest pir;
  pir.SetTableName(database_users);

  AttributeValue user_id_value;
  user_id_value.SetS(user_id);
  pir.AddItem("id", user_id_value);

  AttributeValue login_value;
  login_value.SetS(user.login);
  pir.AddItem("login", login_value);

  AttributeValue node_id_value;
  node_id_value.SetS(user.node_id);
  pir.AddItem("node_id", node_id_value);

  AttributeValue type_value;
  type_value.SetS(user.type);
  pir.AddItem("type", type_value);

  AttributeValue name_value;
  name_value.SetS(user.name);
  pir.AddItem("name", name_value);

  AttributeValue company_value;
  company_value.SetS(user.company);
  pir.AddItem("company", company_value);

  AttributeValue blog_value;
  blog_value.SetS(user.blog);
  pir.AddItem("blog", blog_value);

  AttributeValue location_value;
  location_value.SetS(user.location);
  pir.AddItem("location", location_value);

  AttributeValue email_value;
  email_value.SetS(user.email);
  pir.AddItem("email", email_value);

  AttributeValue hireable_value;
  hireable_value.SetBool(user.hireable);
  pir.AddItem("hireable", hireable_value);

  AttributeValue bio_value;
  bio_value.SetS(user.bio);
  pir.AddItem("bio", bio_value);

  AttributeValue created_at_value;
  created_at_value.SetS(user.created_at);
  pir.AddItem("created_at", created_at_value);

  AttributeValue updated_at_value;
  updated_at_value.SetS(user.updated_at);
  pir.AddItem("updated_at", updated_at_value);

  AttributeValue public_gists_value;
  public_gists_value.SetN(user.public_gists);
  pir.AddItem("public_gists", public_gists_value);

  AttributeValue public_repos_value;
  public_repos_value.SetN(user.public_repos);
  pir.AddItem("public_repos", public_repos_value);

  AttributeValue following_value;
  following_value.SetN(user.following);
  pir.AddItem("following", following_value);

  AttributeValue followers_value;
  followers_value.SetN(user.followers);
  pir.AddItem("followers", followers_value);

  const PutItemOutcome result = db.dynamo->PutItem(pir);
  if (!result.IsSuccess()) {
    spdlog::error(result.GetError().GetMessage());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int Dynamo::create_org(Org) {
  return EXIT_SUCCESS;
}

std::vector<std::string> Dynamo::list_users() {
  std::vector<std::string> users{};

  ScanRequest req;
  req.SetTableName(database_users);

  const ScanOutcome &result = db.dynamo->Scan(req);
  if (result.IsSuccess()) {
    const Aws::Vector<Aws::Map<Aws::String, AttributeValue>> &items = result.GetResult().GetItems();
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

  std::shuffle(users.begin(), users.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

  if (users.size() > 100) {
    users.resize(100);
  }

  return users;
}

std::vector<std::string> Dynamo::list_orgs() {
  std::vector<std::string> orgs{};
  return orgs;
}

int64_t Dynamo::count_user() {
  int64_t count = 0;

  DescribeTableRequest dtr;
  dtr.SetTableName(database_users);

  const DescribeTableOutcome &result = db.dynamo->DescribeTable(dtr);

  if (result.IsSuccess()) {
    const TableDescription &td = result.GetResult().GetTable();
    count = td.GetItemCount();
  }

  return count;
}

int64_t Dynamo::count_org() {
  return 0;
}

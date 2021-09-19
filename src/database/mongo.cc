#include <database/mongo.h>

#define GET_CONNECTION(database_name, collection_name) \
  auto connect = this->pool->acquire();                \
  auto database = connect->database(database_name);    \
  auto coll = database[collection_name];

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::document::value;
using mongocxx::result::insert_one;
using mongocxx::stdx::optional;

Mongo::Mongo(const std::string &dsn) {
  this->dsn = dsn;
}

int Mongo::initialize() {
  mongocxx::instance instance{};
  this->uri = new mongocxx::uri(dsn);
  this->pool = new mongocxx::pool(*this->uri);
  return 0;
}

Mongo::~Mongo() {
  delete this->pool;
  delete this->uri;
}

int Mongo::create_user(User user) {
  try {
    GET_CONNECTION(this->uri->database(), "users")
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = make_document(
        kvp("id", user.id),
        kvp("login", user.login),
        kvp("node_id", user.node_id),
        kvp("type", user.type),
        kvp("name", user.name),
        kvp("company", user.company),
        kvp("blog", user.blog),
        kvp("location", user.location),
        kvp("email", user.email),
        kvp("hireable", user.hireable),
        kvp("bio", user.bio),
        kvp("created_at", user.created_at),
        kvp("updated_at", user.updated_at),
        kvp("public_gists", user.public_gists),
        kvp("public_repos", user.public_repos),
        kvp("following", user.following),
        kvp("followers", user.followers));

    bsoncxx::document::view view = doc_value.view();

    optional<value> val = coll.find_one_and_update(make_document(kvp("id", user.id)), view);
    if (!val) {
      optional<insert_one> result = coll.insert_one(view);
      if (!result) {
        return SQL_EXEC_ERROR;
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occured: {}", e.what());
  }
  return 0;
}

std::vector<User> Mongo::list_usersx(common_args args) {
  std::vector<User> users;
  try {
    GET_CONNECTION(this->uri->database(), "users")
    auto option = mongocxx::options::find{};

    bsoncxx::document::value option_sort = bsoncxx::builder::stream::document{} << "id" << 1 << bsoncxx::builder::stream::finalize;
    option.sort(option_sort.view());
    option.skip((int64_t)(args.page - 1) * args.limit);
    option.limit(args.limit);
    mongocxx::cursor _val = coll.find(bsoncxx::document::view{});
    for (auto &&doc : _val) {
      std::cout << doc["login"].get_utf8().value << "\n";
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occured: {}", e.what());
  }
  return users;
}

int Mongo::create_org(Org) {
  return EXIT_SUCCESS;
}

std::vector<std::string> Mongo::list_users() {
  std::vector<std::string> users;
  try {
    GET_CONNECTION(this->uri->database(), "users")
    mongocxx::pipeline stages;
    stages.sample(2);
    auto option = mongocxx::options::aggregate{};
    option.max_time(std::chrono::milliseconds(5000));
    auto cursor = coll.aggregate(stages, option);

    for (auto doc : cursor) {
      users.emplace_back(doc["login"].get_utf8().value.data());
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occured: {}", e.what());
  }
  return users;
}

std::vector<std::string> Mongo::list_orgs() {
  std::vector<std::string> orgs;
  return orgs;
}

int64_t Mongo::count_user() {
  GET_CONNECTION(this->uri->database(), "users")
  return coll.count_documents({});
}

int64_t Mongo::count_org() {
  GET_CONNECTION(this->uri->database(), "users")
  return coll.count_documents({});
}

int Mongo::create_emoji(std::vector<Emoji> emojis) {
  return EXIT_SUCCESS;
}

int64_t Mongo::count_emoji() {
  return 0;
}

int64_t Mongo::count_gitignore() {
  return 0;
}

int Mongo::create_gitignore(Gitignore gitignore) {
  return EXIT_SUCCESS;
}

int64_t Mongo::count_license() {
  return 0;
}

int Mongo::create_license(License license) {
  return EXIT_SUCCESS;
}

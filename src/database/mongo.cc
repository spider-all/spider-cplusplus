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
  try {
    mongocxx::instance instance{};
    this->uri = new mongocxx::uri(dsn);
    this->pool = new mongocxx::pool(*this->uri);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return DATABASE_OPEN_ERROR;
  }
  return 0;
}

Mongo::~Mongo() {
  delete this->pool;
  delete this->uri;
}

int64_t Mongo::count_x(const std::string &c) {
  GET_CONNECTION(this->uri->database(), c)
  return coll.count_documents({});
}

int Mongo::create_user(User user) {
  try {
    GET_CONNECTION(this->uri->database(), "users")
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
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

std::vector<User> Mongo::list_usersx(common_args args) {
  std::vector<User> users;
  try {
    GET_CONNECTION(this->uri->database(), "users")
    auto option = mongocxx::options::find{};

    bsoncxx::document::value option_sort = bsoncxx::builder::stream::document{} << "id" << 1 << bsoncxx::builder::stream::finalize;
    option.sort(option_sort.view());
    option.skip((args.page - 1) * args.limit);
    option.limit(args.limit);
    mongocxx::cursor _val = coll.find(bsoncxx::document::view{});
    for (auto &&doc : _val) {
      std::cout << doc["login"].get_utf8().value << "\n";
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return users;
}

int Mongo::create_org(Org org) {
  try {
    GET_CONNECTION(this->uri->database(), "orgs")
    bsoncxx::document::value doc_value = make_document(
        kvp("id", org.id),
        kvp("login", org.login),
        kvp("node_id", org.node_id),
        kvp("description", org.description));

    bsoncxx::document::view view = doc_value.view();

    optional<value> val = coll.find_one_and_update(make_document(kvp("id", org.id)), view);
    if (!val) {
      optional<insert_one> result = coll.insert_one(view);
      if (!result) {
        return SQL_EXEC_ERROR;
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
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
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return users;
}

std::vector<std::string> Mongo::list_orgs() {
  std::vector<std::string> orgs;
  return orgs;
}

int64_t Mongo::count_user() {
  return this->count_x("users");
}

int64_t Mongo::count_org() {
  return this->count_x("orgs");
}

int Mongo::create_emoji(std::vector<Emoji> emojis) {
  try {
    GET_CONNECTION(this->uri->database(), "emojis")
    for (const Emoji &emoji : emojis) {
      bsoncxx::document::value doc_value = make_document(
          kvp("name", emoji.name),
          kvp("url", emoji.url));

      bsoncxx::document::view view = doc_value.view();

      optional<value> val = coll.find_one_and_update(make_document(kvp("name", emoji.name)), view);
      if (!val) {
        optional<insert_one> result = coll.insert_one(view);
        if (!result) {
          return SQL_EXEC_ERROR;
        }
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int64_t Mongo::count_emoji() {
  return this->count_x("emojis");
}

int64_t Mongo::count_gitignore() {
  return this->count_x("gitignores");
}

int Mongo::create_gitignore(Gitignore gitignore) {
  try {
    GET_CONNECTION(this->uri->database(), "gitignores")
    bsoncxx::document::value doc_value = make_document(
        kvp("name", gitignore.name),
        kvp("source", gitignore.source));

    bsoncxx::document::view view = doc_value.view();

    optional<value> val = coll.find_one_and_update(make_document(kvp("name", gitignore.name)), view);
    if (!val) {
      optional<insert_one> result = coll.insert_one(view);
      if (!result) {
        return SQL_EXEC_ERROR;
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int64_t Mongo::count_license() {
  return this->count_x("licenses");
}

int Mongo::create_license(License license) {
  try {
    GET_CONNECTION(this->uri->database(), "licenses")
    bsoncxx::document::value doc_value = make_document(
        kvp("key", license.key),
        kvp("name", license.name),
        kvp("spdx_id", license.spdx_id),
        kvp("node_id", license.node_id),
        kvp("description", license.description),
        kvp("implementation", license.implementation),
        kvp("permissions", license.permissions),
        kvp("conditions", license.conditions),
        kvp("limitations", license.limitations),
        kvp("body", license.body),
        kvp("featured", license.featured));

    bsoncxx::document::view view = doc_value.view();

    optional<value> val = coll.find_one_and_update(make_document(kvp("key", license.key)), view);
    if (!val) {
      optional<insert_one> result = coll.insert_one(view);
      if (!result) {
        return SQL_EXEC_ERROR;
      }
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

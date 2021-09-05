#include <database/mongo.h>

Mongo::Mongo(const std::string &string) {
  mongocxx::instance instance{};
  mongocxx::uri uri(string);
  db.client = new mongocxx::client(uri);
  auto database = db.client->database("vmapp");
  coll = database.collection("test");
}

int Mongo::initialize() {
  return 0;
}

Mongo::~Mongo() {
  if (db.client != nullptr) {
    delete db.client;
  }
}

int Mongo::create_user(User user) {
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = builder
                                       << "id" << user.id
                                       << "login" << user.login
                                       << "node_id" << user.node_id
                                       << "type" << user.type
                                       << "name" << user.name
                                       << "company" << user.company
                                       << "blog" << user.blog
                                       << "location" << user.location
                                       << "email" << user.email
                                       << "hireable" << user.hireable
                                       << "bio" << user.bio
                                       << "created_at" << user.created_at
                                       << "updated_at" << user.updated_at
                                       << "public_gists" << user.public_gists
                                       << "public_repos" << user.public_repos
                                       << "following" << user.following
                                       << "followers" << user.followers
                                       << bsoncxx::builder::stream::finalize;
  bsoncxx::document::view view = doc_value.view();
  mongocxx::v_noabi::stdx::optional<bsoncxx::document::value> val = coll.find_one_and_update(
      bsoncxx::builder::stream::document{} << "id"
                                           << user.id
                                           << bsoncxx::builder::stream::finalize,
      view);
  if (!val) {
    mongocxx::v_noabi::stdx::optional<mongocxx::v_noabi::result::insert_one> result = coll.insert_one(view);
    if (!result) {
      return SQL_EXEC_ERROR;
    }
  }
  return 0;
}

int Mongo::create_org(Org) {
  return EXIT_SUCCESS;
}

std::vector<std::string> Mongo::list_users() {
  std::vector<std::string> users;
  return users;
}

std::vector<std::string> Mongo::list_orgs() {
  std::vector<std::string> orgs;
  return orgs;
}

int64_t Mongo::count_user() { return 0; }
int64_t Mongo::count_org() { return 0; }

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

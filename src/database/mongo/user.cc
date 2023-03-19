#include <database/mongo.h>

int Mongo::upsert_user(User user) {
  bsoncxx::types::b_date now(std::chrono::system_clock::now());
  bsoncxx::document::value doc = make_document(
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
      kvp("followers", user.followers),
      kvp("x_upserted_at", now));
  bsoncxx::document::value filter = make_document(kvp("id", user.id));
  return this->upsert_x("users", bsoncxx::to_json(filter), bsoncxx::to_json(doc));
}

int Mongo::upsert_user_with_version(User user, enum request_type type) {
  WRAP_FUNC(this->upsert_user(user))
  WRAP_FUNC(this->update_version(std::to_string(user.id), type))
  return EXIT_SUCCESS;
}

std::vector<User> Mongo::list_usersx(common_args args) {
  std::vector<User> users;
  try {
    GET_CONNECTION(this->uri->database(), "users")
    mongocxx::cursor _val = coll.find(bsoncxx::document::view{});
    for (auto &&doc : _val) {
      std::cout << doc["login"].get_string().value << "\n";
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return users;
}

std::vector<std::string> Mongo::list_users_random(enum request_type type) {
  return this->list_x_random("users", "login", type);
}

int64_t Mongo::count_user() {
  return this->count_x("users");
}

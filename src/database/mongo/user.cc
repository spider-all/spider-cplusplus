#include <database/mongo.h>

int Mongo::upsert_user(User user) {
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
      kvp("followers", user.followers));
  bsoncxx::document::value filter = make_document(kvp("id", user.id));
  return this->upsert_x("users", filter.view(), doc.view());
}

int Mongo::upsert_user_with_version(User user, enum request_type type) {
  WRAP_FUNC(this->upsert_user(user))
  WRAP_FUNC(this->update_version(user.login, type))
  return EXIT_SUCCESS;
}

std::vector<User> Mongo::list_usersx(common_args args) {
  std::vector<User> users;
  try {
    GET_CONNECTION(this->uri->database(), "users")
    mongocxx::cursor _val = coll.find(bsoncxx::document::view{});
    for (auto &&doc : _val) {
      std::cout << doc["login"].get_utf8().value << "\n";
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return users;
}

std::vector<std::string> Mongo::list_users_random(enum request_type type) {
  std::string type_string = this->versions->to_string(type);

  std::vector<std::string> users;
  try {
    std::string func_name = this->function_name_helper(__func__);
    GET_CONNECTION(this->uri->database(), func_name)

    mongocxx::pipeline stages;
    stages.lookup(make_document(
        kvp("from", fmt::format("{}_version", type_string)),
        kvp("localField", "login"),
        kvp("foreignField", "login"),
        kvp("as", fmt::format("{}_version", type_string))));

    int64_t version = this->versions->get(type);

    bsoncxx::document::view_or_value filter1 = make_document(kvp(fmt::format("{}_version.version", type_string), make_document(kvp("$lt", version))));
    bsoncxx::document::view_or_value filter2 = make_document(kvp(fmt::format("{}_version", type_string), make_document(kvp("$size", 0))));
    stages.match(make_document(kvp("$or", make_array(filter1, filter2))));

    stages.sample(this->sample_size);

    mongocxx::options::aggregate option;
    option.max_time(std::chrono::milliseconds(5000));
    auto cursor = coll.aggregate(stages, option);
    for (auto &&doc : cursor) {
      users.emplace_back(doc["login"].get_utf8().value.data());
    }
    spdlog::info("version: {}", version);
    if (users.empty()) {
      this->incr_version(type);
    } else {
      this->update_version(users, type);
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return users;
}

int64_t Mongo::count_user() {
  return this->count_x("users");
}

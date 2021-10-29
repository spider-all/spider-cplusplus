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
  int code = this->upsert_user(user);
  if (code != 0) {
    return code;
  }
  code = this->update_version(user.login, type);
  if (code != 0) {
    return code;
  }
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

int Mongo::upsert_org(Org org) {
  bsoncxx::document::value doc = make_document(
      kvp("id", org.id),
      kvp("login", org.login),
      kvp("node_id", org.node_id),
      kvp("description", org.description));
  bsoncxx::document::value filter = make_document(kvp("id", org.id));
  return this->upsert_x("orgs", filter.view(), doc.view());
}

int Mongo::upsert_org_with_version(Org org, enum request_type type) {
  int code = this->upsert_org(org);
  if (code != 0) {
    return code;
  }
  code = this->update_version(org.login, type);
  if (code != 0) {
    return code;
  }
  return EXIT_SUCCESS;
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

std::vector<std::string> Mongo::list_orgs_random(enum request_type type) {
  std::string type_string = request_type_string(type);

  std::vector<std::string> orgs;
  try {
    std::string func_name = this->function_name_helper(__func__);
    GET_CONNECTION(this->uri->database(), func_name)

    mongocxx::pipeline stages;
    stages.lookup(make_document(
        kvp("from", fmt::format("{}_version", type_string)),
        kvp("localField", "login"),
        kvp("foreignField", "login"),
        kvp("as", fmt::format("{}_version", type_string))));

    stages.unwind(make_document(kvp("path", fmt::format("${}_version", type_string))));

    int64_t version = this->versions->get(type);

    stages.match(make_document(kvp(fmt::format("{}_version", type_string), make_document(kvp("$lt", version)))));

    stages.sample(this->sample_size);

    mongocxx::options::aggregate option;
    option.max_time(std::chrono::milliseconds(5000));
    auto cursor = coll.aggregate(stages, option);
    for (auto &&doc : cursor) {
      orgs.emplace_back(doc["login"].get_utf8().value.data());
    }
    if (orgs.empty()) {
      version = this->versions->incr(type);
      auto coll = database["versions"];
      mongocxx::options::update option;
      option.upsert(true);
      coll.update_one(make_document(kvp("type", type)),
                      make_document(
                          kvp("$set", make_document(
                                          kvp("type", type),
                                          kvp("version", version)))),
                      option);
    } else {
      for (auto org : orgs) {
        auto coll = database[fmt::format("{}_version", request_type_string(type))];
        mongocxx::options::update option;
        option.upsert(true);
        int64_t version = this->versions->get(type);
        coll.update_one(make_document(kvp("login", org)),
                        make_document(
                            kvp("$set",
                                make_document(kvp("login", org),
                                              kvp("version", version)))),
                        option);
      }
    }

  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return orgs;
}

int Mongo::create_emoji(std::vector<Emoji> emojis) {
  try {
    GET_CONNECTION(this->uri->database(), "emojis")
    coll.drop();
    std::vector<bsoncxx::document::value> docs;
    for (const Emoji &emoji : emojis) {
      bsoncxx::document::value doc = make_document(kvp("name", emoji.name), kvp("url", emoji.url));
      docs.push_back(doc);
    }
    coll.insert_many(docs);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int Mongo::create_gitignore(Gitignore gitignore) {
  try {
    GET_CONNECTION(this->uri->database(), "gitignores")
    bsoncxx::document::value doc_value = make_document(
        kvp("name", gitignore.name),
        kvp("source", gitignore.source));
    mongocxx::options::update option;
    option.upsert(true);
    coll.update_one(make_document(kvp("name", gitignore.name)), make_document(kvp("$set", doc_value)), option);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int Mongo::create_license(License license, enum request_type type) {
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
    mongocxx::options::update option;
    option.upsert(true);
    coll.update_one(make_document(kvp("key", license.key)), make_document(kvp("$set", doc_value)), option);

    {
      auto coll = database[fmt::format("{}_version", request_type_string(type))];
      mongocxx::options::update option;
      option.upsert(true);
      int64_t version = this->versions->get(type);
      coll.update_one(make_document(kvp("key", license.key)),
                      make_document(
                          kvp("$set",
                              make_document(kvp("key", license.key),
                                            kvp("version", version)))),
                      option);
    }

  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int Mongo::create_repo(Repo repo, enum request_type type) {
  try {
    GET_CONNECTION(this->uri->database(), "repos")
    bsoncxx::document::value doc_value = make_document(
        kvp("id", repo.id),
        kvp("node_id", repo.node_id),
        kvp("name", repo.name),
        kvp("full_name", repo.full_name),
        kvp("xprivate", repo.xprivate),
        kvp("owner", repo.owner),
        kvp("owner_type", repo.owner_type),
        kvp("fork", repo.fork),
        kvp("created_at", repo.created_at),
        kvp("updated_at", repo.updated_at),
        kvp("pushed_at", repo.pushed_at),
        kvp("homepage", repo.homepage),
        kvp("size", repo.size),
        kvp("stargazers_count", repo.stargazers_count),
        kvp("watchers_count", repo.watchers_count),
        kvp("forks_count", repo.forks_count),
        kvp("language", repo.language),
        kvp("license", repo.license),
        kvp("forks", repo.forks),
        kvp("open_issues", repo.open_issues),
        kvp("watchers", repo.watchers),
        kvp("default_branch", repo.default_branch));
    mongocxx::options::update option;
    option.upsert(true);
    coll.update_one(make_document(kvp("id", repo.id)), make_document(kvp("$set", doc_value)), option);

    {
      auto coll = database[fmt::format("{}_version", request_type_string(type))];
      mongocxx::options::update option;
      option.upsert(true);
      int64_t version = this->versions->get(type);
      coll.update_one(make_document(kvp("id", repo.id)),
                      make_document(
                          kvp("$set",
                              make_document(kvp("id", repo.id),
                                            kvp("version", version)))),
                      option);
    }

  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return EXIT_SUCCESS;
}

int64_t Mongo::count_repo() {
  return this->count_x("repos");
}

int64_t Mongo::count_license() {
  return this->count_x("licenses");
}

int64_t Mongo::count_emoji() {
  return this->count_x("emojis");
}

int64_t Mongo::count_gitignore() {
  return this->count_x("gitignores");
}

int64_t Mongo::count_user() {
  return this->count_x("users");
}

int64_t Mongo::count_org() {
  return this->count_x("orgs");
}
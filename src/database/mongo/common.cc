#include <database/mongo.h>

std::string Mongo::function_name_helper(std::string func_name) {
  std::vector<std::string> result;
  result = boost::split(result, func_name, boost::is_any_of("_"));
  if (result.size() < 2) {
    return "";
  }
  return result[1];
}

int Mongo::upsert_x(std::string collection, bsoncxx::document::view_or_value filter,
                    bsoncxx::document::view_or_value update) {
  mongocxx::options::update option;
  option.upsert(true);
  try {
    GET_CONNECTION(this->uri->database(), collection)
    coll.update_one(filter, make_document(kvp("$set", update)), option);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

int64_t Mongo::count_x(const std::string &c) {
  GET_CONNECTION(this->uri->database(), c)
  return coll.count_documents({});
}

int Mongo::update_version(std::string key, enum request_type type) {
  int64_t version = this->versions->get(type);

  mongocxx::options::update option;
  option.upsert(true);

  try {
    GET_CONNECTION(this->uri->database(), "users")
    coll = database[fmt::format("{}_version", request_type_string(type))];

    bsoncxx::document::value record = make_document(kvp("key", key), kvp("version", version));
    coll.update_one(make_document(kvp("key", key)), make_document(kvp("$set", record)), option);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

int Mongo::update_version(std::vector<std::string> keys, enum request_type type) {
  int64_t version = this->versions->get(type);

  try {
    GET_CONNECTION_RAW(this->uri->database())
    auto coll = database[fmt::format("{}_version", request_type_string(type))];
    auto bulk = coll.create_bulk_write();
    for (std::string key : keys) {
      bsoncxx::document::value record = make_document(kvp("key", key), kvp("version", version));
      mongocxx::model::update_one upsert_op{make_document(kvp("key", key)), make_document(kvp("$set", record))};
      upsert_op.upsert(true);
      bulk.append(upsert_op);
    }
    auto result = bulk.execute();
    if (!result) {
      spdlog::error("Something mongodb error occurred: {}", "bulk execute result is null");
      return SQL_EXEC_ERROR;
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

int Mongo::incr_version(enum request_type type) {
  int64_t version = this->versions->incr(type);

  mongocxx::options::update option;
  option.upsert(true);
  try {
    GET_CONNECTION(this->uri->database(), "users")
    coll = database["versions"];
    std::string type_string = this->versions->to_string(type);
    auto filter = make_document(kvp("type", type_string));
    auto doc = make_document(kvp("$set", make_document(kvp("type", type_string), kvp("version", version))));
    coll.update_one(filter.view(), doc.view(), option);
    spdlog::info("Increase {} to {}", fmt::format("{}_version", type_string), version);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

std::vector<std::string> Mongo::list_x_random(std::string collection, std::string key, enum request_type type) {
  std::string type_string = this->versions->to_string(type);

  std::vector<std::string> result;

  mongocxx::pipeline stages;
  stages.lookup(make_document(
      kvp("from", fmt::format("{}_version", type_string)),
      kvp("localField", key),
      kvp("foreignField", "key"),
      kvp("as", fmt::format("{}_version", type_string))));

  int64_t version = this->versions->get(type);

  bsoncxx::document::view_or_value filter1 = make_document(kvp(fmt::format("{}_version.version", type_string), make_document(kvp("$lt", version))));
  bsoncxx::document::view_or_value filter2 = make_document(kvp(fmt::format("{}_version", type_string), make_document(kvp("$size", 0))));
  stages.match(make_document(kvp("$or", make_array(filter1, filter2))));

  stages.sample(this->sample_size);

  mongocxx::options::aggregate option;
  option.max_time(std::chrono::milliseconds(5000));

  try {
    GET_CONNECTION(this->uri->database(), collection)
    auto cursor = coll.aggregate(stages, option);
    for (auto &&doc : cursor) {
      result.emplace_back(doc[key].get_utf8().value.data());
    }
    if (result.empty()) {
      this->incr_version(type);
    } else {
      this->update_version(result, type);
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
  }
  return result;
}

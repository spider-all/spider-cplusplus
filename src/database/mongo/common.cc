#include <database/mongo.h>

int Mongo::insert_x(const std::string &collection, bsoncxx::document::view_or_value doc) {
  try {
    GET_CONNECTION(this->uri->database(), collection)
    coll.insert_one(doc.view());
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

int Mongo::upsert_x(const std::string &collection, std::string filter, std::string update) {
  mongocxx::options::update option;
  option.upsert(true);
  try {
    bsoncxx::document::value filter_doc = bsoncxx::from_json(filter);
    bsoncxx::document::value update_doc = bsoncxx::from_json(update);
    GET_CONNECTION(this->uri->database(), collection)
    coll.update_one(std::move(filter_doc), make_document(kvp("$set", update_doc)), option);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

int Mongo::upsert_x(const std::string &collection, const std::map<std::string, std::string> &filters) {
  try {
    GET_CONNECTION(this->uri->database(), collection)
    auto bulk = coll.create_bulk_write();
    for (auto &it : filters) {
      bsoncxx::document::value filter = bsoncxx::from_json(it.first);
      bsoncxx::document::value doc = bsoncxx::from_json(it.second);
      mongocxx::model::update_one upsert_op{filter.view(), make_document(kvp("$set", doc))};
      upsert_op.upsert(true);
      bulk.append(upsert_op);
    }
    bulk.execute();
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
    coll = database[fmt::format("{}_version", this->versions->to_string(type))];

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
    auto coll = database[fmt::format("{}_version", this->versions->to_string(type))];
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

// list_x_random
// @params
//    keys name$string:id$int64 代表获取 name 字段类型为 string, id 字段类型为 int64 的数据
std::vector<std::string> Mongo::list_x_random(const std::string &collection, std::string keys, enum request_type type) {
  std::string type_string = this->versions->to_string(type);

  std::vector<std::string> result;

  std::vector<std::string> params{keys};
  std::string key = keys;
  if (boost::algorithm::contains(keys, ":")) {
    key = keys.substr(0, keys.find(":"));
    boost::algorithm::split(params, keys, boost::algorithm::is_any_of(":"));
  }

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
      std::string res;
      bool first = true;
      for (auto param : params) {
        std::string s;
        if (boost::algorithm::contains(param, "$")) {
          std::vector<std::string> param_list;
          boost::algorithm::split(param_list, param, boost::algorithm::is_any_of("$"));
          if (param_list.size() != 2) {
            spdlog::error("Something mongodb error occurred: {}", "parameter is not correct");
            return result;
          }
          auto doc_param = doc[param_list[0]];
          if (param_list[1] == "string") {
            s = doc_param.get_utf8().value.to_string();
          } else if (param_list[1] == "int") {
            s = std::to_string(doc_param.get_int64().value);
          } else if (param_list[1] == "double") {
            s = std::to_string(doc_param.get_double().value);
          } else if (param_list[1] == "int64") {
            s = std::to_string(doc_param.get_int64().value);
          } else if (param_list[1] == "int32") {
            s = std::to_string(doc_param.get_int32().value);
          } else {
            spdlog::error("Something mongodb error occurred: {}", "parameter is not correct");
            return result;
          }
        } else {
          auto doc_param = doc[param];
          if (doc_param.type() == bsoncxx::type::k_utf8) {
            s = doc_param.get_utf8().value.to_string();
          } else {
            spdlog::error("Something mongodb error occurred: {}", "parameter is not correct");
            return result;
          }
        }
        if (first) {
          res = s;
          first = false;
        } else {
          res += ":" + s;
        }
      }
      result.push_back(res);
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

int Mongo::ensure_index(const std::string &collection, std::vector<std::string> keys) {
  try {
    std::string name = fmt::format("{}_index", boost::algorithm::join(keys, "_"));
    GET_CONNECTION(this->uri->database(), collection)
    auto cursor = coll.list_indexes();
    for (auto &&doc : cursor) {
      if (doc["name"].get_utf8().value.to_string() == name) {
        spdlog::info("Collection {} index {} already exists", collection, name);
        return EXIT_SUCCESS;
      }
    }
    mongocxx::options::index index_options{};
    index_options.unique(true);
    index_options.name(name);
    auto doc = bsoncxx::builder::basic::document{};
    for (const auto &key : keys) {
      doc.append(kvp(key, 1));
    }
    auto result = coll.create_index(doc.view(), index_options);
    auto result_view = result.view();
    if (result_view.find("name") != result_view.end()) {
      spdlog::info("Collection {} create index {} success", collection, result_view["name"].get_utf8().value.to_string());
      return EXIT_SUCCESS;
    }
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

std::string bson_type(std::string str) {
  if (str == "string") {
    return "string";
  } else if (str == "int32") {
    return "int";
  } else if (str == "int64") {
    return "long";
  } else if (str == "double") {
    return "double";
  }
  return "string";
}

// create_x_collection
// @params
//    keys name$string:id$int64 代表获取 name 字段类型为 string, id 字段类型为 int64 的数据
int Mongo::create_x_collection(const std::string &collection, std::string keys) {
  if (keys.empty()) {
    return EXIT_SUCCESS;
  }
  std::vector<std::string> params{keys};
  if (boost::algorithm::contains(keys, ":")) {
    boost::algorithm::split(params, keys, boost::algorithm::is_any_of(":"));
  }

  try {
    GET_CONNECTION_RAW(this->uri->database())
    auto cursor = database.list_collections();
    for (auto &&doc : cursor) {
      if (doc["name"].get_utf8().value.to_string() == collection) {
        spdlog::info("Collection {} already exists", collection);
        return EXIT_SUCCESS;
      }
    }
    auto doc = bsoncxx::builder::basic::document{};
    for (const auto &param : params) {
      if (boost::algorithm::contains(param, "$")) {
        std::vector<std::string> param_list;
        boost::algorithm::split(param_list, param, boost::algorithm::is_any_of("$"));
        if (param_list.size() != 2) {
          spdlog::error("Something mongodb error occurred: {}", "parameter is not correct");
          return SQL_EXEC_ERROR;
        }
        doc.append(kvp(param_list[0], make_document(kvp("bsonType", bson_type(param_list[1])), kvp("description", fmt::format("must be a {} and is required", bson_type(param_list[1]))))));
      } else {
        doc.append(kvp(param, make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))));
      }
    }
    mongocxx::options::create_collection create_collection_options;
    mongocxx::validation_criteria validation_criteria;
    validation_criteria.rule(doc.view());
    validation_criteria.level(mongocxx::validation_criteria::validation_level::k_strict);
    validation_criteria.action(mongocxx::validation_criteria::validation_action::k_error);
    create_collection_options.validation_criteria(validation_criteria);
    database.create_collection(collection, create_collection_options);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return SQL_EXEC_ERROR;
  }
  return EXIT_SUCCESS;
}

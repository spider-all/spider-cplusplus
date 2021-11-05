#include <database/mongo.h>

int Mongo::create_collections() {
  {
    auto properties = make_document(
        kvp("id", make_document(kvp("bsonType", "long"), kvp("description", "must be a int64_t and is required"))),
        kvp("name", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("login", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("node_id", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("type", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("created_at", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("updated_at", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("public_gists", make_document(kvp("bsonType", "long"), kvp("description", "must be a int64_t and is required"))),
        kvp("public_repos", make_document(kvp("bsonType", "long"), kvp("description", "must be a int64_t and is required"))),
        kvp("following", make_document(kvp("bsonType", "long"), kvp("description", "must be a int64_t and is required"))),
        kvp("followers", make_document(kvp("bsonType", "long"), kvp("description", "must be a int64_t and is required"))));
    auto fields_required = make_array("id", "name", "login", "node_id", "type", "created_at", "updated_at", "public_gists", "public_repos", "following", "followers");
    auto schema = make_document(kvp("bsonType", "object"),
                                kvp("required", fields_required),
                                kvp("properties", properties));
    auto doc = make_document(kvp("$jsonSchema", schema));
    WRAP_FUNC(this->create_x_collection("users", doc.view()))
  }
  {
    auto properties = make_document(
        kvp("id", make_document(kvp("bsonType", "long"), kvp("description", "must be a int64_t and is required"))),
        kvp("login", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("node_id", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))));
    auto fields_required = make_array("id", "login", "node_id");
    auto schema = make_document(kvp("bsonType", "object"),
                                kvp("required", fields_required),
                                kvp("properties", properties));
    auto doc = make_document(kvp("$jsonSchema", schema));
    WRAP_FUNC(this->create_x_collection("orgs", doc.view()))
  }
  {
    auto properties = make_document(
        kvp("name", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("url", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))));
    auto fields_required = make_array("name", "url");
    auto schema = make_document(kvp("bsonType", "object"),
                                kvp("required", fields_required),
                                kvp("properties", properties));
    auto doc = make_document(kvp("$jsonSchema", schema));
    WRAP_FUNC(this->create_x_collection("emojis", doc.view()))
  }
  {
    auto properties = make_document(
        kvp("name", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("source", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))));
    auto fields_required = make_array("name", "source");
    auto schema = make_document(kvp("bsonType", "object"),
                                kvp("required", fields_required),
                                kvp("properties", properties));
    auto doc = make_document(kvp("$jsonSchema", schema));
    WRAP_FUNC(this->create_x_collection("gitignores", doc.view()))
  }
  {
    auto properties = make_document(
        kvp("key", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))),
        kvp("name", make_document(kvp("bsonType", "string"), kvp("description", "must be a string and is required"))));
    auto fields_required = make_array("key", "name");
    auto schema = make_document(kvp("bsonType", "object"),
                                kvp("required", fields_required),
                                kvp("properties", properties));
    auto doc = make_document(kvp("$jsonSchema", schema));
    WRAP_FUNC(this->create_x_collection("licenses", doc.view()))
  }
  return EXIT_SUCCESS;
}

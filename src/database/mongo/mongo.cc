#include <database/mongo.h>

Mongo::Mongo(const std::string &dsn) {
  this->dsn = dsn;
}

Mongo::~Mongo() {
  delete this->pool;
  delete this->uri;
}

int Mongo::initialize() {
  this->versions = new Versions();
  try {
    mongocxx::instance instance{};
    this->uri = new mongocxx::uri(dsn);
    this->pool = new mongocxx::pool(*this->uri);
  } catch (const std::exception &e) {
    spdlog::error("Something mongodb error occurred: {}", e.what());
    return DATABASE_OPEN_ERROR;
  }
  WRAP_FUNC(this->create_collections())
  return this->initialize_version();
}

int Mongo::initialize_version() {
  GET_CONNECTION(this->uri->database(), "versions")
  mongocxx::cursor _val = coll.find(bsoncxx::document::view{});
  this->versions->initialize(std::move(_val));
  return EXIT_SUCCESS;
}

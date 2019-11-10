cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE ../pkgs/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(spider)

add_executable(spider cli.cc common.cc config.cc request.cc server.cc spider.cc dbsq.cc dbrk.cc)

target_include_directories(spider PRIVATE inc)

find_package(sqlite3 CONFIG REQUIRED)
target_link_libraries(spider PRIVATE sqlite3)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE yaml-cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(spider PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(ZLIB REQUIRED)
target_link_libraries(spider PRIVATE ZLIB::ZLIB)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(spider PRIVATE spdlog::spdlog)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(spider PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(RocksDB CONFIG REQUIRED)
target_link_libraries(spider PRIVATE RocksDB::rocksdb RocksDB::rocksdb-shared)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE SQLiteCpp)

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

# find_package(RocksDB CONFIG REQUIRED)
# target_link_libraries(main PRIVATE RocksDB::rocksdb RocksDB::rocksdb-shared)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE ../pkgs/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(spider)

include_directories(.)

find_package(sqlite3 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
# find_package(CURL CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(spider cli.cc common.cc config.cc request.cc server.cc spider.cc dbsq.cc dbrk.cc)

target_include_directories(spider PRIVATE inc)

target_link_libraries(spider PRIVATE nlohmann_json nlohmann_json::nlohmann_json)
target_link_libraries(spider PRIVATE spdlog::spdlog)
target_link_libraries(spider PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(spider PRIVATE yaml-cpp)
target_link_libraries(spider PRIVATE ZLIB::ZLIB)
target_link_libraries(spider PRIVATE sqlite3)
# target_link_libraries(spider PRIVATE CURL::libcurl)

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

# find_package(RocksDB CONFIG REQUIRED)
# target_link_libraries(main PRIVATE RocksDB::rocksdb RocksDB::rocksdb-shared)

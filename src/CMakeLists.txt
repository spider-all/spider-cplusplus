cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_TOOLCHAIN_FILE ../pkgs/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fsanitize=address,signed-integer-overflow")

project(spider)

FILE(GLOB Source *.cc application/*.cc database/*.cc)
add_executable(spider ${Source})

if (CMAKE_BUILD_TYPE STREQUAL release)
  add_custom_command(TARGET spider POST_BUILD COMMAND strip spider)
endif ()

target_include_directories(spider PRIVATE inc)

find_package(sqlite3 CONFIG REQUIRED)
target_link_libraries(spider PRIVATE sqlite3)

target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libhiredis.a)

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

find_package(cpr CONFIG REQUIRED)
target_link_libraries(spider PRIVATE cpr)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CURL::libcurl)

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

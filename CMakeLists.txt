cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE pkgs/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Os")
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

add_definitions(-DSPIDER_VERSION=${SPIDER_VERSION})

project(spider)

FILE(GLOB source src/*.cc src/application/*.cc src/database/*.cc)
add_executable(spider ${source})

if (UNIX AND NOT APPLE)
  target_link_libraries(spider PRIVATE stdc++fs)
endif ()

target_link_libraries(spider PRIVATE stdc++ pthread dl)

if (CMAKE_BUILD_TYPE STREQUAL release)
  add_custom_command(TARGET spider POST_BUILD COMMAND strip spider)
endif ()

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CLI11::CLI11)

find_package(aws-cpp-sdk-core CONFIG REQUIRED)
target_link_libraries(spider PRIVATE aws-cpp-sdk-core)

find_package(aws-cpp-sdk-dynamodb CONFIG REQUIRED)
target_link_libraries(spider PRIVATE aws-cpp-sdk-dynamodb)

find_package(aws-cpp-sdk-kinesis CONFIG REQUIRED)
target_link_libraries(spider PRIVATE aws-cpp-sdk-kinesis)

find_package(aws-cpp-sdk-s3 CONFIG REQUIRED)
target_link_libraries(spider PRIVATE aws-cpp-sdk-s3)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(spider PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE yaml-cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(spider PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libmongocxx-static.a)
target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libmongoc-1.0.a)
target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libbsoncxx-static.a)
target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libbson-static-1.0.a)

target_link_libraries(spider PRIVATE sasl2)
target_link_libraries(spider PRIVATE resolv)

target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libzstd.a)

find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(spider PRIVATE libpqxx::pqxx)

find_package(libfort CONFIG REQUIRED)
target_link_libraries(spider PRIVATE libfort::fort)

find_package(ZLIB REQUIRED)
target_link_libraries(spider PRIVATE ZLIB::ZLIB)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(spider PRIVATE spdlog::spdlog)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE SQLiteCpp)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CURL::libcurl)

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
target_include_directories(spider PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})

find_package(leveldb CONFIG REQUIRED)
target_link_libraries(spider PRIVATE leveldb::leveldb)

find_package(libmysql REQUIRED)
target_link_libraries(spider PRIVATE ${MYSQL_LIBRARIES})

find_package(absl CONFIG REQUIRED)
target_link_libraries(spider PRIVATE absl::any absl::base absl::bits absl::city)

target_include_directories(spider PRIVATE include)

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

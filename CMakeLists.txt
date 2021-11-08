cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_DIR}pkgs/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Os")

add_definitions(-DSPIDER_VERSION=${SPIDER_VERSION})

project(spider)

FILE(GLOB source src/*.cc src/application/*.cc src/application/request/*.cc src/database/mongo/*.cc)
add_executable(spider ${source})

if (CMAKE_BUILD_TYPE STREQUAL release)
  add_custom_command(TARGET spider POST_BUILD COMMAND strip spider)
endif ()

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CLI11::CLI11)

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

target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libgumbo.a)

target_link_libraries(spider PRIVATE sasl2)
target_link_libraries(spider PRIVATE resolv)

target_link_libraries(spider PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libzstd.a)

find_package(libfort CONFIG REQUIRED)
target_link_libraries(spider PRIVATE libfort::fort)

find_package(ZLIB REQUIRED)
target_link_libraries(spider PRIVATE ZLIB::ZLIB)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(spider PRIVATE fmt::fmt-header-only)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(spider PRIVATE spdlog::spdlog)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CURL::libcurl)

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
target_include_directories(spider PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})

find_package(prometheus-cpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE prometheus-cpp::core prometheus-cpp::pull)

target_include_directories(spider PRIVATE include)

target_link_libraries(spider PRIVATE stdc++ pthread dl)

if (UNIX AND NOT APPLE)
  target_link_libraries(spider PRIVATE rt stdc++fs)
endif ()

find_package(GTest CONFIG REQUIRED)

function(spider_test)
  cmake_parse_arguments(PARSED_ARGS "" "NAME" "SRCS" ${ARGN})
  add_executable(${PARSED_ARGS_NAME} ${PARSED_ARGS_SRCS})
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libmongocxx-static.a)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libmongoc-1.0.a)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libbsoncxx-static.a)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libbson-static-1.0.a)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE sasl2)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE resolv)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libzstd.a)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE ZLIB::ZLIB)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE fmt::fmt-header-only)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE spdlog::spdlog)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
  target_link_libraries(${PARSED_ARGS_NAME} PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
  if (UNIX AND NOT APPLE)
    target_link_libraries(${PARSED_ARGS_NAME} PRIVATE rt stdc++fs)
  endif ()
  target_include_directories(${PARSED_ARGS_NAME} PRIVATE include)
endfunction()

if (NOT CMAKE_BUILD_TYPE STREQUAL release)
  FILE(GLOB test_list_x_random test/list_x_random.cc src/database/mongo/*.cc src/versions.cc)
  spider_test(
    NAME test_list_x_random
    SRCS ${test_list_x_random}
  )
  FILE(GLOB test_ensure_index test/ensure_index.cc src/database/mongo/*.cc src/versions.cc)
  spider_test(
    NAME test_ensure_index
    SRCS ${test_ensure_index}
  )
  FILE(GLOB test_create_x_collection test/create_x_collection.cc src/database/mongo/*.cc src/versions.cc)
  spider_test(
    NAME test_create_x_collection
    SRCS ${test_create_x_collection}
  )
endif()

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

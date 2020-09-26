cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")

set(SQLiteCpp_version 3.1.1)
set(cpr_version 1.5.1)
set(nlohmann_json_version 3.9.1)
set(yaml_cpp_version 6.3.0)

project(spider)

FILE(GLOB Source *.cc application/*.cc database/*.cc)
add_executable(spider ${Source})

target_compile_definitions(spider PRIVATE cpr_version="${cpr_version}")
target_compile_definitions(spider PRIVATE yaml_cpp_version="${yaml_cpp_version}")

if (APPLE)
  set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/opt/openssl)
  set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
  set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib)
  set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/3rdparty/SQLiteCpp-${SQLiteCpp_version})

set(BUILD_CPR_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/3rdparty/cpr-${cpr_version})

target_include_directories(spider PRIVATE ${CMAKE_CURRENT_LIST_DIR}/3rdparty/SQLiteCpp-${SQLiteCpp_version}/include)
target_include_directories(spider PRIVATE ${CMAKE_CURRENT_LIST_DIR}/3rdparty/cpr-${cpr_version}/include)
target_include_directories(spider PRIVATE ${CMAKE_CURRENT_LIST_DIR}/3rdparty/nlohmann-json-${nlohmann_json_version}/include)

target_include_directories(spider PRIVATE include)

if (APPLE)
  target_include_directories(spider PRIVATE /usr/local/opt/spdlog/include)
  target_include_directories(spider PRIVATE /usr/local/opt/fmt/include)
  target_include_directories(spider PRIVATE /usr/local/opt/nlohmann-json/include)
  target_include_directories(spider PRIVATE /usr/local/opt/yaml-cpp/include)
  target_include_directories(spider PRIVATE /usr/local/opt/boost/include)
  target_include_directories(spider PRIVATE /usr/local/opt/hiredis/include)
  target_include_directories(spider PRIVATE /usr/local/opt/zlib/include)
  target_include_directories(spider PRIVATE /usr/local/opt/leveldb/include)
  target_include_directories(spider PRIVATE /usr/local/opt/curl/include)
endif()

if (APPLE)
  target_link_directories(spider PRIVATE /usr/local/opt/spdlog/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/fmt/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/nlohmann-json/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/yaml-cpp/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/sqlite/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/hiredis/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/zlib/lib)
  target_link_directories(spider PRIVATE /usr/local/opt/leveldb/lib)
endif()

target_link_libraries(spider PRIVATE SQLiteCpp sqlite3 leveldb cpr fmt hiredis yaml-cpp curl pthread dl z)

if (CMAKE_BUILD_TYPE STREQUAL release)
  add_custom_command(TARGET spider POST_BUILD COMMAND strip spider)
endif ()

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Os")

add_definitions(-DSPIDER_VERSION=${SPIDER_VERSION})

project(spider)

FILE(GLOB source src/*.cc src/application/*.cc src/application/request/*.cc src/database/sqlite/*.cc)
add_executable(spider ${source})

if (CMAKE_BUILD_TYPE STREQUAL release)
  add_custom_command(TARGET spider POST_BUILD COMMAND strip spider)
endif ()

find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CLI11::CLI11)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(spider PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE yaml-cpp::yaml-cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(spider PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE SQLiteCpp)

find_package(libfort CONFIG REQUIRED)
target_link_libraries(spider PRIVATE libfort::fort)

find_package(ZLIB REQUIRED)
target_link_libraries(spider PRIVATE ZLIB::ZLIB)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(spider PRIVATE fmt::fmt-header-only)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(spider PRIVATE spdlog::spdlog_header_only)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(spider PRIVATE CURL::libcurl)

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
target_include_directories(spider PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})

find_package(prometheus-cpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE prometheus-cpp::core prometheus-cpp::pull)

find_package(croncpp CONFIG REQUIRED)
target_link_libraries(spider PRIVATE croncpp::croncpp)

find_package(utf8proc CONFIG REQUIRED)
target_link_libraries(spider PRIVATE utf8proc::utf8proc)

target_include_directories(spider PRIVATE include)

# target_link_libraries(spider PRIVATE stdc++ pthread dl)

if (UNIX AND NOT APPLE)
  target_link_libraries(spider PRIVATE rt stdc++fs)
endif ()

if (APPLE)
  target_link_libraries(spider PRIVATE "-framework CoreFoundation")
  target_link_libraries(spider PRIVATE "-framework Security")
endif ()

message("System info: ${CMAKE_SYSTEM}")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("System host name: ${CMAKE_HOST_SYSTEM_NAME}")
message("CXX Standard: ${CMAKE_CXX_STANDARD}")

cmake_minimum_required(VERSION 3.16)
project(MultiplyTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# =============== main project ============================

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB src_files "${PROJECT_SOURCE_DIR}/src/*.hpp"
                    "${PROJECT_SOURCE_DIR}/src/*.cpp"
                    "${PROJECT_SOURCE_DIR}/src/*/*.hpp"
                    "${PROJECT_SOURCE_DIR}/src/*/*.cpp")

add_executable(simple-tcp-server ${src_files} main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# =============== test project ===========================
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${googletest_SOURCE_DIR}/googletest/include/gtest)

file(GLOB test_files "${PROJECT_SOURCE_DIR}/tests/*.hpp"
                    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
                    "${PROJECT_SOURCE_DIR}/tests/*/*.hpp"
                    "${PROJECT_SOURCE_DIR}/tests/*/*.cpp")

add_executable(simple-tcp-server-tests ${src_files} ${test_files})

target_link_libraries(simple-tcp-server-tests GTest::GTest)

add_test(simple-tcp-server-tests test)

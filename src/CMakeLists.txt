cmake_minimum_required(VERSION 3.16)
project(simple-tcp-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/src)

#======================= Simple TCP Server =====================

file(GLOB src_files "${PROJECT_SOURCE_DIR}/src/*.hpp"
                    "${PROJECT_SOURCE_DIR}/src/*.cpp"
                    "${PROJECT_SOURCE_DIR}/src/*/*.hpp"
                    "${PROJECT_SOURCE_DIR}/src/*/*.cpp")

add_executable(simple-tcp-server ${src_files} main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

cmake_minimum_required(VERSION 3.16)
project(MultiplyTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/examples)

#======================= Build Library for Examples ============
set(library_sources
    "${PROJECT_SOURCE_DIR}/src/tcp-server.cpp"
    "${PROJECT_SOURCE_DIR}/src/sockets/epoll-socket.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils/thread-pool.cpp"
)

set(library_headers
    "${PROJECT_SOURCE_DIR}/src/tcp-server.hpp"
    "${PROJECT_SOURCE_DIR}/src/log.hpp"
    "${PROJECT_SOURCE_DIR}/src/constants.hpp"
    "${PROJECT_SOURCE_DIR}/src/sockets/epoll-socket.hpp"
    "${PROJECT_SOURCE_DIR}/src/sockets/isocket.hpp"
    "${PROJECT_SOURCE_DIR}/src/utils/thread-pool.hpp"
)

add_library(simple-tcp-server-lib ${library_sources} ${library_headers})

# =============== simple chat server project ============================

add_executable(simple-chat-server "${PROJECT_SOURCE_DIR}/examples/simple-chat-server.cpp")

target_link_libraries(simple-chat-server PRIVATE simple-tcp-server-lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

cmake_minimum_required(VERSION 3.16)
project(MultiplyTest LANGUAGES CXX)

# Include the CMakeLists.txt for simple-tcp-server
include(src/CMakeLists.txt)

# Include the CMakeLists.txt for simple-tcp-server-tests
include(tests/CMakeLists.txt)

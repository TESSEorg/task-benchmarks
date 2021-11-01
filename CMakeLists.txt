cmake_minimum_required(VERSION 3.19)
project(test-benchmarks LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/")

set(BUILD_TESTING OFF CACHE BOOL "Whether to build tests in this project and its subprojects")

include(FindOrFetchTaskflow)
include(FindOrFetchTTG)

find_package(OpenMP)

add_subdirectory(src)


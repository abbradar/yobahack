cmake_minimum_required(VERSION 2.8)
project(yobahack-tests)

# Common
include(../CMakeCommon.txt)

# Enable testing
enable_testing()

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Source files
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${COMMON_LIBS} ${GTEST_BOTH_LIBRARIES})
GTEST_ADD_TESTS(${PROJECT_NAME} "" ${SRC_LIST})

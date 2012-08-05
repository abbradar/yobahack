cmake_minimum_required(VERSION 2.8)
project(yobahack-client)

# Common
include(../CMakeCommon.txt)

# Source files
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${COMMON_SRCS})
target_link_libraries(${PROJECT_NAME} ${COMMON_LIBS})

# Testing
if(TESTING)
  aux_source_directory(tests TESTS_LIST)
  set(TEST_NAME ${PROJECT_NAME}_test)
  add_executable(${TEST_NAME} ${SRC_LIST} ${COMMON_SRCS} ${TESTS_LIST})
  target_link_libraries(${TEST_NAME} ${COMMON_LIBS} ${GTEST_BOTH_LIBRARIES})
  GTEST_ADD_TESTS(${TEST_NAME} "" ${TESTS_LIST})
endif()

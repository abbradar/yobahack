cmake_minimum_required(VERSION 2.8)
project(yobahack-tests)

# Common
include(../CMakeCommon.txt)

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


## Source files
#aux_source_directory(. SRC_LIST)

#add_executable(super-function-test ${SUPER_FUNCTION_TEST_SOURCES})
#target_link_libraries(super-function-test ${COMMON_LIBS} ${GTEST_BOTH_LIBRARIES})
#GTEST_ADD_TESTS(super-function-test "" ${SUPER_FUNCTION_TEST_SOURCES})

#add_executable(${PROJECT_NAME} ${SRC_LIST})
#target_link_libraries(${PROJECT_NAME} ${COMMON_LIBS})

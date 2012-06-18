cmake_minimum_required(VERSION 2.8)
project(yobahack-client)

# Common
include(../CMakeCommon.txt)

# Source files
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

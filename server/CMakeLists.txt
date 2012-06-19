cmake_minimum_required(VERSION 2.8)
project(yobahack-server)

# Common
include(../CMakeCommon.txt)

# Source files
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${COMMON_SRCS})
target_link_libraries(${PROJECT_NAME} ${COMMON_LIBS})

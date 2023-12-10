cmake_minimum_required (VERSION 3.5)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 ")

add_library(easyjson easyjson.c)
add_executable(easyjson_test test.c)
target_link_libraries(easyjson_test easyjson)
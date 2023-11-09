cmake_minimum_required (VERSION 2.6)
project (easyjson_test C)

add_library(easyjson easyjson.c)
add_executable(easyjson_test test.c)
target_link_libraries(easyjson_test easyjson)
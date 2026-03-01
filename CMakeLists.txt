cmake_minimum_required(VERSION 3.16)

project(inplace-library CXX)

add_library(inplace-library INTERFACE)
target_include_directories(inplace-library INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

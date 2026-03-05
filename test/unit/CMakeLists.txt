cmake_minimum_required(VERSION 3.16)

include(../../CMakeLists.txt)

project(inplace-library-unittest CXX)

find_package(GTest REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

file(GLOB_RECURSE TEST_SOURCE_FILES "*_test.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${TEST_SOURCE_FILES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE inplace-library GTest::gmock_main)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wold-style-cast -Wshadow -Wconversion -Werror)


if (NOT DEFINED INPLACE_INSTRUMENTATION OR INPLACE_INSTRUMENTATION STREQUAL "none")
    # Do nothing.
elseif (${INPLACE_INSTRUMENTATION} STREQUAL "coverage")
    target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC --coverage -fno-inline -Og)
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC --coverage -fno-inline -Og)
elseif (${INPLACE_INSTRUMENTATION} STREQUAL "sanitize-thread")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -fsanitize=thread)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE -fsanitize=thread)
elseif (${INPLACE_INSTRUMENTATION} STREQUAL "sanitize-other")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=pointer-compare -fsanitize=pointer-subtract)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=pointer-compare -fsanitize=pointer-subtract)
else()
    message(FATAL_ERROR "Unsupported instrumentation type: ${INPLACE_INSTRUMENTATION}")
endif()

if (NOT DEFINED INPLACE_PROFILE OR INPLACE_PROFILE STREQUAL "default")
    # Do nothing.
elseif (${INPLACE_PROFILE} STREQUAL "constrained")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -fno-exceptions -fno-rtti)
else()
    message(FATAL_ERROR "Unsupported profile: ${INPLACE_PROFILE}")
endif()


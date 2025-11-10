cmake_minimum_required(VERSION 3.22)

project(gnode-root)

option(GNODE_ENABLE_EXAMPLES "" ON)
option(GNODE_ENABLE_TESTS "" ON)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# --- dependencies

find_package(spdlog REQUIRED)

add_subdirectory(external)

# --- library

add_subdirectory(GNode)

# --- everything else...

if(GNODE_ENABLE_EXAMPLES)
  add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif(GNODE_ENABLE_EXAMPLES)

if(GNODE_ENABLE_TESTS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif(GNODE_ENABLE_TESTS)

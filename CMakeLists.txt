cmake_minimum_required(VERSION 3.22)

project(gnode-root)

option(GNODE_ENABLE_EXAMPLES "" OFF)
option(GNODE_ENABLE_TESTS "" OFF)
option(GNODE_ENABLE_UNIT_TESTS "" OFF)

# -----------------------------------------------------------------------------
# Status output
# -----------------------------------------------------------------------------

message(STATUS "┌─ GNode configuration ────────────────")
message(STATUS "│ GNODE_ENABLE_EXAMPLES:   ${GNODE_ENABLE_EXAMPLES}")
message(STATUS "│ GNODE_ENABLE_TESTS:      ${GNODE_ENABLE_TESTS}")
message(STATUS "│ GNODE_ENABLE_UNIT_TESTS: ${GNODE_ENABLE_UNIT_TESTS}")
message(STATUS "└─────────────────────────────────────")

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

if(GNODE_ENABLE_UNIT_TESTS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests/unit_tests)
endif(GNODE_ENABLE_UNIT_TESTS)

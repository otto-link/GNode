project(
  gnode
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION
    "A generic node-based data structure for node graph programming in C++.")

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(${PROJECT_NAME} demekgraph spdlog::spdlog)

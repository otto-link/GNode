cmake_minimum_required(VERSION 3.10)
project(gnode VERSION 0.0.1 LANGUAGES CXX DESCRIPTION "A generic node-based data structure for node graph programming in C++.")

set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")


# sources
file(GLOB_RECURSE SOURCES
     "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                             ${PROJECT_SOURCE_DIR}/include
			     ${PROJECT_SOURCE_DIR}/external/macro-logger
			   PRIVATE
			     )

# Link libraries
target_link_libraries(${PROJECT_NAME})

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
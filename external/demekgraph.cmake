project(demekgraph)

set(DMK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} INTERFACE)
add_library(gnode::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE ${DMK_DIR})

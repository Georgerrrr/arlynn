cmake_minimum_required(VERSION 3.8)

set(SOURCE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
)

add_subdirectory(core)
add_subdirectory(gui)

set(SOURCES ${SOURCES} PARENT_SCOPE)
set(SOURCE_HEADERS ${SOURCE_HEADERS} PARENT_SCOPE)

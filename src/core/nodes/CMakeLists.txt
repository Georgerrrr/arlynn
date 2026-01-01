cmake_minimum_required(VERSION 3.30)

project(core)

list(APPEND CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/output.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/oscillator.cpp 
)

set(CORE_SOURCES ${CORE_SOURCES} PARENT_SCOPE)

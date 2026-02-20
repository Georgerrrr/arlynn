cmake_minimum_required(VERSION 3.30)

list(APPEND SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/output.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/oscillator.cpp 
)

set(SOURCES ${SOURCES} PARENT_SCOPE)

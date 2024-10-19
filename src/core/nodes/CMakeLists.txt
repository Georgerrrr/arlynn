cmake_minimum_required(VERSION 3.8)

project(core)

list(APPEND CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/oscillator.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/amplifier.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/audio-amplitude.cpp
)

set(CORE_SOURCES ${CORE_SOURCES} PARENT_SCOPE)

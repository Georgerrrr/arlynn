cmake_minimum_required(VERSION 3.30)

list(APPEND CORE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/portaudioclient.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/dummyclient.cpp 
)

set(CORE_SOURCES ${CORE_SOURCES} PARENT_SCOPE)

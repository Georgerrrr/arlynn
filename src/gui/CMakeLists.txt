cmake_minimum_required(VERSION 3.30)
project(arlynn)

list(APPEND SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/app.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/mainframe.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/canvas.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/canvasnode.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/canvascontrol.cpp 
)

set(SOURCES ${SOURCES} PARENT_SCOPE)

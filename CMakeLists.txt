cmake_minimum_required(VERSION 3.30)

project(arlynn LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_SCRIPTS_DIR "${CMAKE_SOURCE_DIR}/cmake/scripts")

include("${CMAKE_SCRIPTS_DIR}/debug-variables.cmake")
include("${CMAKE_SCRIPTS_DIR}/libraries.cmake")

add_subdirectory(arl)
add_subdirectory(test)

execute_process(COMMAND ${Python3_EXECUTABLE} -m venv ${CMAKE_SOURCE_DIR}/.venv)
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/.venv/bin/pip install -r ${CMAKE_SOURCE_DIR}/requirements.txt)

set(PYTHON_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/.venv/lib/python3.14/site-packages/")
install(TARGETS arl DESTINATION ${PYTHON_LIBRARY_DIR})

file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)


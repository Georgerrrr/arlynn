cmake_minimum_required(VERSION 3.8)
project(arlynn)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_SCRIPTS_DIR "${CMAKE_SOURCE_DIR}/cmake/scripts")

include("${CMAKE_SCRIPTS_DIR}/debug-variables.cmake")
include("${CMAKE_SCRIPTS_DIR}/libraries.cmake")

include(${wxWidgets_USE_FILE})

if (WIN32)
  set(RESOURCE_PATH ${CMAKE_SOURCE_DIR}/resources/windows)
else()
  set(RESOURCE_PATH ${CMAKE_SOURCE_DIR}/resources/linux)
endif()

set(NODE_PATH ${CMAKE_SOURCE_DIR}/resources/nodes)

if (${CMAKE_BUILD_TYPE})
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(BUILD_DEBUG)
  endif()
endif()

configure_file(${CMAKE_SOURCE_DIR}/src/configure.h.in ${CMAKE_SOURCE_DIR}/src/configure.h)

add_subdirectory(src)

add_executable(arlynn WIN32 ${SOURCES})

target_link_libraries(arlynn PRIVATE core ${wxWidgets_LIBRARIES} ${FFTW3_fftw3_LIBRARY} ${pugixml_LIBRARIES})
target_include_directories(arlynn PUBLIC ${wxWidgets_INCLUDE_DIRS} ${SOURCE_HEADERS} ${pugixml_INCLUDE_DIR})

set_property(TARGET arlynn PROPERTY CXX_STANDARD 20)
set_property(TARGET arlynn PROPERTY CXX_STANDARD_REQUIRED on)

set_property(TARGET arlynn PROPERTY CMAKE_EXPORT_COMPILE_COMMANDS on)

file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)

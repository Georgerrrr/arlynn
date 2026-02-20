cmake_minimum_required(VERSION 3.30)

project(arl LANGUAGES CXX)

list(APPEND SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/audioengine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
)

add_subdirectory(audioclients)
add_subdirectory(nodes)

add_library(core STATIC ${SOURCES})

target_link_libraries(core PRIVATE ${Portaudio_LIBRARY} ${RTMIDI_LIBRARIES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) 

set_property(TARGET core PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET core PROPERTY CXX_STANDARD 20)
set_property(TARGET core PROPERTY CXX_STANDARD_REQUIRED ON)


pybind11_add_module(arl ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(arl PRIVATE core ${Python3_LIBRARIES})
target_include_directories(arl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${pybind11_INCLUDE_DIR} ${Python3_INCLUDE_DIRS})

set_property(TARGET arl PROPERTY CXX_STANDARD 20)
set_property(TARGET arl PROPERTY CXX_STANDARD_REQUIRED ON)

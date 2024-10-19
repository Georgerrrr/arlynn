cmake_minimum_required(VERSION 3.8)

project(core)

list(APPEND CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/audioengine.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/project.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/signal.cpp 
)

add_subdirectory(nodes)
add_subdirectory(audioclients)

add_library(core STATIC ${CORE_SOURCES})

if (WIN32)
  set(Portaudio_LIBRARY portaudio)
endif() 


target_link_libraries(core PRIVATE ${Portaudio_LIBRARY} ${RTMIDI_LIBRARIES})
target_include_directories(core PUBLIC ${SOURCE_HEADERS})

set_property(TARGET core PROPERTY CXX_STANDARD 20)
set_property(TARGET core PROPERTY CXX_STANDARD_REQUIRED on)


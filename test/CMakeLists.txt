cmake_minimum_required(VERSION 3.30)

project(unit_tests)

include(FetchContent)
fetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests
	run.cpp
  fifo.cpp
  benchmark.cpp
)

target_link_libraries(unit_tests PRIVATE core Catch2WithMain)
target_include_directories(unit_tests PRIVATE "${CMAKE_SOURCE_DIR}/src")

set_property(TARGET unit_tests PROPERTY CXX_STANDARD 20)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)

include(Catch)

catch_discover_tests(unit_tests)


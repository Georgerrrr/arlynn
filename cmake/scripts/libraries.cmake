find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

find_package(Portaudio REQUIRED)
find_package(RTMIDI REQUIRED)
find_package(FFTW3 REQUIRED)

include(FetchContent)

fetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG v3.0.1 
)

FetchContent_MakeAvailable(pybind11)

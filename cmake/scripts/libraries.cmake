find_package("Portaudio" REQUIRED)
find_package("RTMIDI" REQUIRED)
find_package("wxWidgets" REQUIRED core base OPTIONAL_COMPONENTS)
find_package("FFTW3" REQUIRED)

include(FetchContent)
fetchContent_Declare(
  pugixml
  GIT_REPOSITORY https://github.com/zeux/pugixml
  GIT_TAG v1.14
)

FetchContent_MakeAvailable(pugixml)
set(pugixml_INCLUDE_DIR ${pugixml_SOURCE_DIR}/src)
set(pugixml_LIBRARIES pugixml)

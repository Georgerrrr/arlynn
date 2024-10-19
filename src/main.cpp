#include <wx/wx.h>

#include "core/audioengine.h"
#include "gui/app.h"

int run(int argc, char** argv) {
  core::AudioEngine::get().initialize(48000, 128);
  wxDISABLE_DEBUG_SUPPORT();
  return wxEntry(argc, argv);
}

#if defined(__WXMAC__) || defined(__WXGTK__)

wxIMPLEMENT_APP_NO_MAIN(gui::App);

int main(int argc, char** argv) {
  return run(argc, argv);
}

#else 

wxIMPLEMENT_APP_NO_MAIN(gui::App);

extern "C" int WINAPI WinMain(
    HINSTANCE hInstance HINSTANCE hPrevInstance, wxCmdLineArgType lpCmdLine, int nCmdShow)
{
  return run(__argc, __argv);
}

#endif

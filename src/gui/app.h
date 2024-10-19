#pragma once 

#include <wx/wx.h>

#include "mainframe.h"

namespace gui {

  class App : public wxApp {
    public:
    App() = default;
    virtual bool OnInit();
    virtual int OnExit();

    private:
    MainFrame* m_mainFrame;
  };

} // namespace gui

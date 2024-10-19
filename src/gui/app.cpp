#include "app.h"

#include "core/audioengine.h"

namespace gui {

  bool App::OnInit() {
    wxInitAllImageHandlers();

    m_mainFrame = new MainFrame();
    m_mainFrame->Show();

    return true;
  }

  int App::OnExit() {
    return 0;
  }

} // namespace gui 

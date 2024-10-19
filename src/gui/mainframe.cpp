#include <wx/wrapsizer.h>

#include "mainframe.h"

#include "core/project.h"

namespace gui {

  MainFrame::MainFrame() 
    : wxFrame(NULL, wxID_ANY, "arlynn", wxDefaultPosition, wxDefaultSize) 
  {
    wxSplitterWindow* splitter = new wxSplitterWindow(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxSP_BORDER | wxSP_LIVE_UPDATE);

    splitter->SetMinimumPaneSize(FromDIP(110));

    auto canvasWrapper = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    const wxColour lightBackground = "#E5E0E0";
    const wxColour darkBackground = "#3F3D3D";
    canvasWrapper->SetBackgroundColour(
        wxSystemSettings::GetAppearance().IsDark() ? darkBackground : lightBackground);

    auto canvasSizer = new wxBoxSizer(wxHORIZONTAL);
    m_nodeCanvas = new Canvas(canvasWrapper);
    m_nodeCanvas->SetBackgroundColour(wxColour("#FFFFFF"));
    canvasSizer->Add(m_nodeCanvas, 1, wxEXPAND | wxALL, FromDIP(5));
    canvasWrapper->SetSizerAndFit(canvasSizer);

    auto controls = makeControlPanel(splitter);
    
    splitter->SplitHorizontally(canvasWrapper, controls);
    splitter->SetSashGravity(1);
    splitter->SetSashPosition(FromDIP(110));

    SetSize(FromDIP(800), FromDIP(500));

    auto outputNode = std::make_shared<core::nodes::Output>();
    m_nodeCanvas->addNode((NODE_PATH "/output.xml"), outputNode);
    core::Project::get().setOutputNode(outputNode);
  }

  MainFrame::~MainFrame() {
  }

  wxPanel* MainFrame::makeControlPanel(wxWindow* parent) {
    const wxColour lightBackground = "#D4D3D3";
    const wxColour darkBackground = "#2C2828";

    auto panel = new wxScrolled<wxPanel>(parent, wxID_ANY);
    panel->SetScrollRate(0, FromDIP(10));
    panel->SetBackgroundColour(
        wxSystemSettings::GetAppearance().IsDark() ? darkBackground : lightBackground);

    auto sizer = new wxWrapSizer(wxHORIZONTAL);
    for (auto& b : m_nodeButtons) {
      auto button = new wxButton(panel, wxID_ANY, b.text, wxDefaultPosition, wxSize(100, 100));
      button->Bind(wxEVT_LEFT_UP, b.onClick);
      sizer->Add(button, 0, wxALL, FromDIP(5));
    }
    panel->SetSizer(sizer);

    return panel;
  }

} // namespace gui

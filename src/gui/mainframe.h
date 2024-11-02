#pragma once 

#include <filesystem>
#include <vector>

#include <wx/wx.h>
#include <wx/splitter.h>

#include "configure.h"
#include "canvas.h"

#include "core/nodes/output.h"
#include "core/nodes/oscillator.h"
#include "core/nodes/amplifier.h"
#include "core/nodes/audio-amplitude.h"

namespace gui {

  class MainFrame : public wxFrame {
    public:
    MainFrame();
    ~MainFrame();

    private:
    wxPanel* makeControlPanel(wxWindow* parent);

    Canvas* m_nodeCanvas;

    const std::filesystem::path m_nodePath = NODE_PATH;

    struct NodeButton {
      wxString text;
      std::function<void(wxMouseEvent&)> onClick;
    };
    const std::vector<NodeButton> m_nodeButtons = {
      NodeButton{
        "Oscillator",
        [this](wxMouseEvent& evt)
        { m_nodeCanvas->addNode((m_nodePath / "oscillator.xml"), std::make_shared<core::nodes::Oscillator>()); }},
      NodeButton{
        "Amplifier",
        [this](wxMouseEvent& evt)
        { m_nodeCanvas->addNode((m_nodePath / "amplifier.xml"), std::make_shared<core::nodes::Amplifier>()); }},
      NodeButton{
        "Audio To\nAmplitude",
        [this](wxMouseEvent& evt)
        { m_nodeCanvas->addNode((m_nodePath / "audio-amplitude.xml"), std::make_shared<core::nodes::AudioToAmplitude>()); }}
    };

  }; 

} // namespace gui 

#pragma once 

#include <memory>
#include <vector>
#include <variant>

#include <wx/wx.h>
#include <wx/graphics.h>

#include <pugixml.hpp>

#include "core/node.h"

namespace gui {

  class CanvasControl;
  class CanvasNode;

  struct InputSlot {
    wxString name;
    wxColour colour;
    wxRect rect;
    size_t index;
    std::shared_ptr<CanvasNode> connection = nullptr;
    size_t connectionIndex = 0;
  };
  struct OutputSlot {
    wxString name;
    wxColour colour;
    wxRect rect;
    size_t index;
  };
  typedef std::variant<InputSlot, OutputSlot> slot_t;

  class CanvasNode {

    public:
    CanvasNode(const wxPoint& position, const std::string& xmlPath, std::shared_ptr<core::Node> node);
    ~CanvasNode();

    void onUpdate(wxGraphicsContext* gc);
    void drawSlot(wxGraphicsContext* gc, const slot_t& slot, size_t index);

    const std::vector<slot_t> getSlots() const { return m_slots; }

    std::vector<std::unique_ptr<CanvasControl>>& getControls() { return m_controls; }

    void setConnection(size_t index, std::shared_ptr<CanvasNode> other, size_t otherIndex);

    void setSelected(bool selected) {
      m_selected = selected;
    }

    void move(const wxPoint& vect) {
      m_rect.x += vect.x;
      m_rect.y += vect.y;
    }

    const wxRect& getRect() const { return m_rect; }

    core::attribute_t getAttribute(size_t index) { return m_node->getAttribute(index); }

    bool topBar(const wxPoint& pos) {
      return wxRect(m_rect.x, m_rect.y, m_rect.width, m_titleBarLine).Contains(pos);
    }

    private:

    wxString m_name;
    wxString m_author;
    wxString m_version;
    wxColour m_colour;
    wxRect m_rect;

    bool m_selected = false;

    static const wxFont m_titleFont;
    static const wxFont m_slotFont;

    static constexpr int m_titleBarLine = 24;
    static constexpr int m_slotTopPadding = 16;
    static constexpr int m_slotDistance = 28;
    static constexpr int m_slotSize = 12;
    static constexpr int m_slotBottomPadding = 16;
    static constexpr int m_slotTextSpacing = 6;
    static constexpr int m_bottomPadding = 8;

    std::shared_ptr<core::Node> m_node;
    std::vector<slot_t> m_slots;
    std::vector<std::unique_ptr<CanvasControl>> m_controls;
    size_t m_inputCount;
    size_t m_outputCount;

    void loadXML(const std::string& xmlPath);
    void loadMetadata(const pugi::xml_node& metadataNode);
    void loadInput(const pugi::xml_node& inputNode);
    void loadOutput(const pugi::xml_node& outputNode);
    void loadAttribute(const pugi::xml_node& attributeNode);
    void getControlPosition(const pugi::xml_node& node, wxRect& rect);

    void resize();

    static const wxColour getColourFromSignal(core::signal_t signal) {
      switch(signal) {
        case (core::signal_t::audio):
          return wxColour("#FE1212");
        case (core::signal_t::amplification):
          return wxColour("#1212FE");
        case (core::signal_t::frequency):
          return wxColour("#12FE12");
        default:
          return wxColour("#FFFFFF");
      }
    }
  };

} // namespace gui

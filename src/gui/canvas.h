#pragma once 

#include <list>
#include <string>
#include <filesystem>
#include <memory>

#include <wx/wx.h>

#include "canvasnode.h"

namespace gui {

  class Canvas : public wxPanel {

    struct Scroll {
      float x = 0.f;
      float y = 0.f;
      float dx = 0.f;
      float dy = 0.f;

      bool scrolling = false;

      wxPoint screenToWorld(const wxPoint& screenPos) {
        return wxPoint(screenPos.x + x, screenPos.y + y);
      }
      void editScroll(const wxPoint& mousePos) {
        x -= (mousePos.x - dx);
        y -= (mousePos.y - dy);
        dx = mousePos.x;
        dy = mousePos.y;
      }
      void reset() {
        x = 0.f;
        y = 0.f;
        dx = 0.f;
        dy = 0.f;
      }
    };

    public:
    Canvas(wxWindow* parent);

    void addNode(const std::filesystem::path& xml, std::shared_ptr<core::Node> node);
    void removeNode(std::shared_ptr<CanvasNode> node);

    static void drawConnection(wxGraphicsContext* gc, const wxPoint& start, const wxPoint& end, const wxColour& colour, direction_t direction=direction_t::input);

    private:
    void onUpdate(wxPaintEvent& evt);

    void onMouseDown(wxMouseEvent& evt);
    void nodeMouseDown(wxMouseEvent& evt);
    void onMouseUp(wxMouseEvent& evt);
    void onMouseLeave(wxMouseEvent& evt);
    void onMouseMove(wxMouseEvent& evt);
    void onMouseScroll(wxMouseEvent& evt);

    void onKeyPress(wxKeyEvent& evt);

    void onContextMenuAppear(wxContextMenuEvent& evt);
    void buildDefaultContextMenu(wxMenu& menu);
    void buildNodeContextMenu(wxMenu& menu);

    Scroll m_scroll;
    wxPoint m_lastMousePos;
    std::list<std::shared_ptr<CanvasNode>> m_nodes;

    Slot* m_link = nullptr;
    bool m_moving = false;
    std::weak_ptr<CanvasNode> m_selectedNode;
  };

} // namespace gui

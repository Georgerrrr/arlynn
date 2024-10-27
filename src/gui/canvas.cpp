#include <cmath>
#include <variant>
#include <memory>

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include "configure.h"
#include "canvas.h"
#include "canvascontrol.h"

namespace gui { 

  Canvas::Canvas(wxWindow* parent) 
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
    , m_lastMousePos(0, 0)
  {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(wxColour("#FFFFFF"));

    Bind(wxEVT_PAINT, &Canvas::onUpdate, this);
    Bind(wxEVT_LEFT_DOWN, &Canvas::onMouseDown, this);
    Bind(wxEVT_MOTION, &Canvas::onMouseMove, this);
    Bind(wxEVT_LEFT_UP, &Canvas::onMouseUp, this);
    Bind(wxEVT_LEAVE_WINDOW, &Canvas::onMouseLeave, this);
    Bind(wxEVT_MOUSEWHEEL, &Canvas::onMouseScroll, this);

    Bind(wxEVT_KEY_DOWN, &Canvas::onKeyPress, this);

    Bind(wxEVT_CONTEXT_MENU, &Canvas::onContextMenuAppear, this);
  }

  void Canvas::addNode(const std::filesystem::path& xml, std::shared_ptr<core::Node> node) {
    m_nodes.push_back(std::make_unique<CanvasNode>(wxPoint(m_scroll.x + 10, m_scroll.y + 10), xml, node));
    Refresh();
  }

  void Canvas::removeNode(std::shared_ptr<CanvasNode> node) {
    if (node->getName() == "Output") return;

    if (!m_selectedNode.expired()) 
      if (m_selectedNode.lock() == node) m_selectedNode.reset();

    for (auto& slot : node->getSlots()) {
      if (slot.direction == direction_t::input) 
        slot.setConnection();
      else if (slot.direction == direction_t::output) {
        for (Slot* connection : slot.connections) {
          connection->setConnection();
        }
      }
    }

    m_nodes.erase(
        std::remove_if(
          m_nodes.begin(), m_nodes.end(),
          [node](const auto& n) { return n == node; }),
        m_nodes.end());

    Refresh();
  }

  void Canvas::onUpdate(wxPaintEvent& evt) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (gc) {
      wxAffineMatrix2D transform{};
      transform.Translate(-m_scroll.x, -m_scroll.y);
      gc->SetTransform(gc->CreateMatrix(transform));

      for (auto& node : m_nodes) {
        node->onUpdate(gc);
      }

      if (m_link != nullptr) {
        wxPoint startPos;
        const wxRect& linkRect = m_link->parent->getRect();
        if (m_link->direction == direction_t::input) {
          startPos.x = linkRect.x - 1;
          startPos.y = 6 + linkRect.y + m_link->rect.y;
        }
        if (m_link->direction == direction_t::output) {
          startPos.x = linkRect.GetRight() + 3;
          startPos.y = 6 + linkRect.y + m_link->rect.y;
        }
        drawConnection(gc, startPos, m_lastMousePos, m_link->colour, m_link->direction);
      }

      delete gc;
    }
  }

  void Canvas::onMouseDown(wxMouseEvent& evt) {
    const wxPoint worldPos = m_scroll.screenToWorld(evt.GetPosition());

    for (auto& n : m_nodes) {
      n->setSelected(false);
      for (auto& c : n->getControls()) {
        if (c->getFocus()) c->setLoseFocus();
      }
    }

    auto clickIter = std::find_if(
        m_nodes.rbegin(), m_nodes.rend(),
        [worldPos](std::shared_ptr<CanvasNode>& node)
        {
          return node->getRect().Contains(worldPos);
        });

    if (clickIter != m_nodes.rend()) {
      auto forwardIter = std::prev(clickIter.base());
      (*forwardIter)->setSelected(true);

      auto d = std::move(*forwardIter);
      m_nodes.erase(forwardIter);
      m_nodes.push_back(std::move(d));
      m_selectedNode = *std::prev(m_nodes.end());
      nodeMouseDown(evt);
    } else {
      m_scroll.scrolling = true;
      m_scroll.dx = evt.GetPosition().x;
      m_scroll.dy = evt.GetPosition().y;
    }

    m_lastMousePos = worldPos;
    Refresh();
    evt.Skip();
  }

  void Canvas::nodeMouseDown(wxMouseEvent& evt) {
    if (m_selectedNode.expired()) return;
    const wxPoint worldPos = m_scroll.screenToWorld(evt.GetPosition());
    auto selected = m_selectedNode.lock();

    if (selected->topBar(worldPos)) {
      m_moving = true;
      return;
    }

    for (auto& control : selected->getControls()) {
      if (control->mouseCollide(selected->getRect(), worldPos)) {
        control->onMouseDown(wxPoint(
              worldPos.x - selected->getRect().x, 
              worldPos.y - selected->getRect().y));
      }
    }

    size_t i = 0;
    for (auto& slot : selected->getSlots()) {
      const wxRect rect(
          selected->getRect().x + slot.rect.x,
          selected->getRect().y + slot.rect.y,
          slot.rect.width, slot.rect.height);
      if (!rect.Contains(worldPos))
        continue;

      slot.setConnection();
      m_link = &slot;
    }
  }

  void Canvas::onMouseUp(wxMouseEvent& evt) {
    m_scroll.scrolling = false;
    m_moving = false;

    const wxPoint worldPos = m_scroll.screenToWorld(evt.GetPosition());

    if (!m_selectedNode.expired()) {
      auto s = m_selectedNode.lock();
      for (auto& control : s->getControls()) {
        control->onMouseUp(wxPoint(
              worldPos.x - s->getRect().x, 
              worldPos.y - s->getRect().y));

        if (control->loseFocusOnMouseUp()) {
          control->setLoseFocus();
        }
      }
    }

    auto clickIter = std::find_if(
        m_nodes.rbegin(), m_nodes.rend(),
        [worldPos](std::shared_ptr<CanvasNode>& node)
        {
          return node->getRect().Contains(worldPos);
        });

    if (clickIter != m_nodes.rend()) {

      if (m_link != nullptr) {
        for(auto& slot : (*clickIter)->getSlots()) {
          if (m_link->direction == slot.direction)
            continue;

          const wxRect rect(
              (*clickIter)->getRect().x + slot.rect.x, 
              (*clickIter)->getRect().y + slot.rect.y,
              slot.rect.width, slot.rect.height);
          if (!rect.Contains(worldPos))
            continue;

          if (slot.direction == direction_t::input) {
            slot.setConnection(m_link);
          }
          else if (slot.direction == direction_t::output) {
            m_link->setConnection(&slot);
          }
        }
      }
    } 

    m_link = nullptr;
    Refresh();
    evt.Skip();
  }
  
  void Canvas::onMouseLeave(wxMouseEvent& evt) {
    onMouseUp(evt);
  }

  void Canvas::onMouseMove(wxMouseEvent& evt) {
    const wxPoint worldPos = m_scroll.screenToWorld(evt.GetPosition());

    if (m_scroll.scrolling) {
      m_scroll.editScroll(evt.GetPosition());
    }

    if (!m_selectedNode.expired()) {
      auto selected = m_selectedNode.lock();
      for (auto& control : selected->getControls()) {
        if (control->getFocus())
          control->onMouseMove(wxPoint(
                worldPos.x - selected->getRect().x, 
                worldPos.y - selected->getRect().y));
      }

      if (m_moving) {
        selected->move(worldPos - m_lastMousePos);
      }
    }

    m_lastMousePos = worldPos;
    Refresh();
  }

  void Canvas::onMouseScroll(wxMouseEvent& evt) {
  }

  void Canvas::onKeyPress(wxKeyEvent& evt) {
    char x = evt.GetUnicodeKey();
    if (!m_selectedNode.expired()) {
      auto selected = m_selectedNode.lock();
      for (auto& control : selected->getControls()) {
        if (control->getFocus()) {
          control->onKeyPress(x);
        }
      }
    }
    Refresh();
    evt.Skip();
  }

  void Canvas::onContextMenuAppear(wxContextMenuEvent& evt) {
    auto position = evt.GetPosition();
    wxMenu menu;

    if (evt.GetPosition() == wxDefaultPosition) {
      position = wxPoint(GetSize().GetWidth() / 2, GetSize().GetHeight() / 2);
      buildDefaultContextMenu(menu);
    } else {
      position = ScreenToClient(evt.GetPosition());
      const wxPoint worldPos = m_scroll.screenToWorld(position);
      auto clickIter = std::find_if(
          m_nodes.rbegin(), m_nodes.rend(),
          [worldPos](auto& node)
          { return node->getRect().Contains(worldPos.x, worldPos.y); });
      if (clickIter == m_nodes.rend())
        buildDefaultContextMenu(menu);
      else {
        for (auto& node : m_nodes)
          node->setSelected(false);
        (*clickIter).get()->setSelected(true);
        m_selectedNode = (*clickIter);
        Refresh();
        buildNodeContextMenu(menu);
      }
    }

    PopupMenu(&menu, position);
    evt.Skip();
  }

  void Canvas::buildDefaultContextMenu(wxMenu& menu) {
    auto clear = menu.Append(wxID_ANY, "New");
    auto save = menu.Append(wxID_ANY, "Save");
    auto saveas = menu.Append(wxID_ANY, "Save As");
    auto load = menu.Append(wxID_ANY, "Open");
    auto viewer = menu.Append(wxID_ANY, "Viewer");
    auto settings = menu.Append(wxID_ANY, "Settings");
  }

  void Canvas::buildNodeContextMenu(wxMenu& menu) {
    auto remove = menu.Append(wxID_ANY, "Delete Node");
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& evt) {
          if (m_selectedNode.expired()) return;
          removeNode(m_selectedNode.lock());
        }, remove->GetId());
  }

  void Canvas::drawConnection(wxGraphicsContext* gc, const wxPoint& start, const wxPoint& end, const wxColour& colour, direction_t direction) {
    auto easeStrength = [](const wxPoint& a, const wxPoint& b, float strength) {
      const auto dx = b.x - a.x;
      const auto dy = b.y - a.y;
      const auto distance = sqrt(dx * dx + dy * dy);
      const auto halfDistance = distance / 2;
      if (halfDistance < strength)
        strength = strength * sin(M_PI * 0.5f * halfDistance / strength);
      return strength;
    };

    const auto startStrength = easeStrength(start, end, 100.f);
    const auto endStrength = easeStrength(start, end, 100.f);
    int8_t dir;
    if (direction == direction_t::input)
      dir = -1;
    if (direction == direction_t::output)
      dir = 1;

    const wxPoint controlA = wxPoint(start.x + (dir * startStrength), start.y);
    const wxPoint controlB = wxPoint(end.x - (dir * endStrength), end.y);
    gc->SetPen(wxPen(colour, 2));
    auto path = gc->CreatePath();
    path.MoveToPoint(start);
    path.AddCurveToPoint(controlA, controlB, end);
    gc->StrokePath(path);
  }

} // namespace gui

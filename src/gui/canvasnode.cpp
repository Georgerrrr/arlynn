#include <algorithm>

#include "core/project.h"

#include "canvas.h"
#include "canvasnode.h"
#include "canvascontrol.h"

namespace gui {

  const wxFont CanvasNode::m_titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  const wxFont CanvasNode::m_slotFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

  CanvasNode::CanvasNode(const wxPoint& position, const std::string& xmlPath, std::shared_ptr<core::Node> node) 
    : m_rect(position.x, position.y, 208, 0)
    , m_node(node)
    , m_inputCount(0)
    , m_outputCount(0)
  {
    core::Project::get().addNode(m_node);
    loadXML(xmlPath);
    resize();
  }

  CanvasNode::~CanvasNode() {
  }

  void CanvasNode::onUpdate(wxGraphicsContext *gc) {
    const auto roundness = 4;
    const auto titlePadding = 2;

    gc->SetPen(*wxWHITE_PEN);
    gc->SetBrush(*wxWHITE_BRUSH);
    gc->DrawRoundedRectangle(m_rect.x, m_rect.y, m_rect.width, m_rect.height, roundness);

    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->SetBrush(wxBrush(m_colour));
    gc->DrawRoundedRectangle(m_rect.x, m_rect.y, m_rect.width, m_titleBarLine, roundness);
    gc->DrawRectangle(m_rect.x, m_rect.y + (m_titleBarLine / 2), m_rect.width, (m_titleBarLine / 2));

    gc->SetPen(m_selected ? *wxBLACK_PEN : *wxLIGHT_GREY_PEN);
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->DrawRoundedRectangle(m_rect.x, m_rect.y, m_rect.width, m_rect.height, roundness);
    const auto titleBarY = m_rect.GetTop() + m_titleBarLine;
    gc->StrokeLine(m_rect.GetLeft(), titleBarY, m_rect.GetRight(), titleBarY);

    gc->SetFont(m_titleFont, *wxBLACK);
    gc->DrawText(m_name, m_rect.GetLeft() + titlePadding + 4, m_rect.GetTop() + titlePadding);

    gc->SetFont(m_slotFont, *wxBLACK);
    size_t index = 0;
    for (auto& slot : m_slots) {
      drawSlot(gc, slot, index);
      index++;
    }

    for (auto& control : m_controls) {
      control->onUpdate(gc, m_rect);
    }
  }

  void CanvasNode::drawSlot(wxGraphicsContext* gc, const Slot& slot, size_t index) {
    gc->SetPen(*wxBLACK_PEN);
    gc->SetBrush(wxBrush(slot.colour));

    gc->DrawRectangle(m_rect.x + slot.rect.x, m_rect.y + slot.rect.y, slot.rect.width, slot.rect.height);

    wxDouble textWidth;
    gc->GetTextExtent(slot.name, &textWidth, NULL);
    wxPoint textPos(m_rect.x + slot.rect.x - textWidth - m_slotTextSpacing, m_rect.y + slot.rect.y - 6);

    if (slot.direction == direction_t::input) {
      textPos.x = m_rect.x + slot.rect.width + m_slotTextSpacing;
    }
    gc->DrawText(slot.name, textPos.x, textPos.y);

    if (slot.direction == direction_t::input && slot.connections[0] != nullptr) {
      auto otherSlot = slot.connections[0];
      const wxPoint start(
          m_rect.x + slot.rect.x - 1,
          m_rect.y + slot.rect.y + 6);
      const wxPoint end(
          otherSlot->parent->getRect().x + otherSlot->rect.GetRight() + 3,
          otherSlot->parent->getRect().y + otherSlot->rect.y + 6);
      Canvas::drawConnection(gc, start, end, slot.colour);
    }
  }

  void CanvasNode::loadXML(const std::string& xmlPath) {
    pugi::xml_document doc;
    auto result = doc.load_file(xmlPath.c_str());
    if (!result) {
      fprintf(stderr, "Error loading %s!\n", xmlPath.c_str());
      fprintf(stderr, "%s\n", result.description());
      return;
    }

    for (pugi::xml_node node : doc.child("Node")) {
      const std::string nodeType = node.name();
      if (!nodeType.compare("metadata")) {
        loadMetadata(node);
      }
      if (!nodeType.compare("input")) {
        loadInput(node);
      }
      if (!nodeType.compare("output")) {
        loadOutput(node);
      }
      if (!nodeType.compare("attribute")) {
        loadAttribute(node);
      }
      resize();
    }
  }

  void CanvasNode::loadMetadata(const pugi::xml_node& metadataNode) {
    for (pugi::xml_node node : metadataNode.children()) {
      const std::string key = node.name();
      if (!key.compare("title")) {
        m_name = node.attribute("value").value();
      }
      if (!key.compare("author")) {
        m_author = node.attribute("value").value();
      }
      if (!key.compare("version")) {
        m_version = node.attribute("value").value();
      }
      if (!key.compare("colour")) {
        m_colour = wxColour(node.attribute("value").value());
      }
    }
  }

  void CanvasNode::loadInput(const pugi::xml_node& inputNode) {
    auto& input = m_node->getInput(m_inputCount);
    const auto yPos = (m_titleBarLine + m_slotTopPadding + (m_slotDistance + m_slotSize) * m_slots.size()); 
    m_slots.push_back(
        Slot(this, direction_t::input, m_inputCount,
          inputNode.attribute("text").value(),
          getColourFromSignal(input.inputType),
          wxRect(0, yPos, m_slotSize, m_slotSize)
        ));
    m_inputCount++;
  }

  void CanvasNode::loadOutput(const pugi::xml_node& outputNode) {
    auto& output = m_node->getOutput(m_outputCount);
    const auto yPos = (m_titleBarLine + m_slotTopPadding + (m_slotDistance + m_slotSize) * m_slots.size()); 
    m_slots.push_back(
        Slot(this, direction_t::output, m_outputCount,
          outputNode.attribute("text").value(),
          getColourFromSignal(core::getOutputFormat(output)),
          wxRect((m_rect.width - m_slotSize), yPos, m_slotSize, m_slotSize)
        ));
    m_outputCount++;
  }

  static bool isDigits(const std::string& str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
  }

  void CanvasNode::loadAttribute(const pugi::xml_node& attributeNode) {
    const std::string control = attributeNode.attribute("control").value();
    const pugi::xml_node& position = attributeNode.child("position");
    wxRect rect;

    if (!control.compare("textbox")) {
      rect.width = position.child("size").attribute("width").as_int();
      rect.height = position.child("size").attribute("height").as_int();
      getControlPosition(position, rect);

      auto newControl = std::make_unique<TextBox>(this, rect, m_controls.size(), attributeNode.child("default").attribute("value").value());
      m_controls.push_back(std::move(newControl));
    } 
    else if (!control.compare("dial")) {
      rect.width = 34;
      rect.height = 34;
      getControlPosition(position, rect);
      auto newControl = std::make_unique<Dial>(this, rect, m_controls.size(),
          wxColour(attributeNode.child("colour").attribute("value").value()),
          attributeNode.child("minimum").attribute("value").as_double(),
          attributeNode.child("default").attribute("value").as_double(),
          attributeNode.child("maximum").attribute("value").as_double(),
          attributeNode.child("micro").attribute("value").as_double());
      m_controls.push_back(std::move(newControl));
    }
    else if (!control.compare("optionbox")) {
      rect.width = position.child("size").attribute("width").as_int();
      rect.height = position.child("size").attribute("height").as_int();
      getControlPosition(position, rect);

      std::vector<OptionBox::Option> options;
      for (auto& node : attributeNode.child("options")) {
        if (std::string("option").compare(node.name())) continue;
        options.push_back(OptionBox::Option(node.attribute("image").value()));
      }

      auto newControl = std::make_unique<OptionBox>(this, rect, m_controls.size(),
          attributeNode.child("options_per_row").attribute("value").as_int(),
          attributeNode.child("default").attribute("value").as_int(),
          options);
      m_controls.push_back(std::move(newControl));
    }
    else {}
  }

  void CanvasNode::getControlPosition(const pugi::xml_node& node, wxRect& rect) {
    const std::string anchor = node.child("anchor").attribute("value").value();
    if (anchor.find("n") != std::string::npos) 
      rect.y = 0;
    if (anchor.find("s") != std::string::npos)
      rect.y = m_rect.height;
    if (anchor.find("e") != std::string::npos)
      rect.x = 0;
    if (anchor.find("w") != std::string::npos)
      rect.x = m_rect.width;
    if (isDigits(anchor)) {
      rect.y = (m_titleBarLine + m_slotTopPadding + (m_slotDistance + m_slotSize) * std::stoi(anchor)) - (rect.height >> 2);
    }

    rect.x += node.child("coordinates").attribute("x").as_int();
    rect.y += node.child("coordinates").attribute("y").as_int();
  }

  void CanvasNode::resize() {
      int totalHeight = m_titleBarLine + m_slotTopPadding + (m_slots.size() * (m_slotDistance + m_slotSize)) - m_slotDistance + m_slotBottomPadding;

      for (auto& control : m_controls) {
        const auto& rect = control->getRect();
        totalHeight = std::max(totalHeight, rect.y + rect.height + m_bottomPadding);
      }
      m_rect.SetHeight(totalHeight);
  }

  Slot::Slot(CanvasNode* parent, direction_t direction, size_t index,
        const wxString& name, const wxColour& colour, const wxRect& rect)
    : parent(parent)
    , direction(direction)
    , index(index)
    , name(name)
    , colour(colour)
    , rect(rect)
  {
    if (direction == direction_t::input)
      connections.resize(1, nullptr);
  }

  void Slot::setConnection(Slot* newConnection) {
    if (direction != direction_t::input) return;

    if (newConnection == nullptr) {
      parent->m_node->setInput(index);
      if (connections[0] != nullptr)
        connections[0]->connections.erase(std::remove_if(connections[0]->connections.begin(),
                              connections[0]->connections.end(),
                              [this](Slot* slot) { return slot == this; }),
                              connections[0]->connections.end());
      connections[0] = nullptr;
      return;
    }
    auto error = parent->m_node->setInput(index, newConnection->parent->m_node, newConnection->index);
    if (error == 0 && newConnection->parent != this->parent) {
      if (connections[0] != nullptr) {
        connections[0]->connections.erase(std::remove_if(connections[0]->connections.begin(),
                              connections[0]->connections.end(),
                              [this](Slot* slot) { return slot == this; }),
                              connections[0]->connections.end());
      }
      connections[0] = newConnection;
      newConnection->connections.push_back(this);
    }
  }

} // namespace gui

#include <cmath>
#include <variant>
#include <mutex>

#include "core/project.h"
#include "core/node.h"

#include "canvasnode.h"
#include "canvascontrol.h"

namespace gui {

  struct StringToAttribute {
    const wxString& string;
    void operator()(int* value) {
      string.ToInt(value);
    }
    void operator()(float* value) {
      *value = wxAtof(string);
    }
    void operator()(double* value) {
      string.ToDouble(value);
    }
  };

  template<class T>
  struct ArithmeticToAttribute {
    static_assert(std::is_arithmetic<T>::value, "Template parameter T must be an arithmetic type!");
    const T input;
    void operator()(int* value) {
      *value = input;
    }
    void operator()(float* value) {
      *value = input;
    }
    void operator()(double* value) {
      *value = input;
    }
  };

  struct AttributeToString {
    wxString operator()(int* value) {
      return wxString::Format(wxT("%i"), *value);
    }
    wxString operator()(float* value) {
      return wxString::Format(wxT("%f"), *value);
    }
    wxString operator()(double* value) {
      return wxString::Format(wxT("%f"), *value);
    }
  };

  template<class T>
  struct AttributeToArithmetic {
    static_assert(std::is_arithmetic<T>::value, "Template parameter T must be an arithmetic type!");
    T operator()(int* value) {
      return *value;
    }
    T operator()(float* value) {
      return *value;
    }
    T operator()(double* value) {
      return *value;
    }
  };

  CanvasControl::CanvasControl(CanvasNode* parent, const wxRect& rect, size_t attributeIndex) 
    : m_parent(parent)
    , m_rect(rect)
    , m_focus(false)
    , m_attributeIndex(attributeIndex)
  {}

  const wxFont TextBox::m_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

  TextBox::TextBox(CanvasNode* parent, const wxRect& rect, size_t attributeIndex)
    : CanvasControl(parent, rect, attributeIndex)
    , m_value(std::visit(AttributeToString{}, parent->getAttribute(attributeIndex)))
  {}

  void TextBox::onKeyPress(const char key) {
    if (key == 0x8) {
      m_value.RemoveLast();
    }
    if (key == 0xD || key == 0x1B) {
      setLoseFocus();
    }
    if (key >= 32) {
      m_value.Append(key);
    }
  }

  void TextBox::onUpdate(wxGraphicsContext* gc, const wxRect& parentRect) const {
    gc->SetPen(m_focus ? *wxBLACK_PEN : *wxLIGHT_GREY_PEN);
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->DrawRoundedRectangle(
        m_rect.x + parentRect.x,
        m_rect.y + parentRect.y,
        m_rect.width, m_rect.height, 1);
    gc->SetFont(m_font, *wxBLACK);
    gc->DrawText(m_value, m_rect.x + parentRect.x + 4, m_rect.y + parentRect.y);
  }

  void TextBox::setLoseFocus() {
    std::scoped_lock<std::mutex> lock(core::Project::get().dataMutex);

    m_focus = false;
    core::attribute_t attribute = m_parent->getAttribute(m_attributeIndex);
    
    std::visit(StringToAttribute{m_value}, attribute);
  }

  Dial::Dial(CanvasNode* parent, const wxRect& rect, size_t attributeIndex, const wxColour& fillColour, double minValue, double maxValue, double microChange) 
    : CanvasControl(parent, rect, attributeIndex)
    , m_fillColour(fillColour)
    , m_minValue(minValue)
    , m_value(std::visit(AttributeToArithmetic<double>{}, parent->getAttribute(attributeIndex)))
    , m_maxValue(maxValue)
    , m_microChange(microChange)
  {
  }

  void Dial::onMouseDown(const wxPoint &pos) {
    m_mousePos = pos;
  }

  void Dial::onMouseMove(const wxPoint &pos) {
    int yDifference = pos.y - m_mousePos.y;

    m_value += ((m_maxValue - m_minValue) / -100.0 * yDifference);
    if (m_value > m_maxValue) m_value = m_maxValue;
    if (m_value < m_minValue) m_value = m_minValue;

    onChange();

    m_mousePos = pos;
  }

  void Dial::onMouseScroll(const int rotation) {
    if (rotation > 0 && m_value < m_maxValue)
      m_value += m_microChange;
    if (rotation < 0 && m_value > m_minValue)
      m_value -= m_microChange;

    onChange();
  }

  void Dial::onUpdate(wxGraphicsContext *gc, const wxRect &parentRect) const {
    const int innerCircleReduction = 8;
    gc->SetPen(m_focus ? *wxBLACK_PEN : *wxGREY_PEN);
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->DrawEllipse(m_rect.x + parentRect.x, m_rect.y + parentRect.y, m_rect.width, m_rect.height);

    gc->SetBrush(*wxGREY_BRUSH);
    gc->DrawEllipse(
        m_rect.x + (innerCircleReduction >> 1) + parentRect.x,
        m_rect.y + (innerCircleReduction >> 1) + parentRect.y,
        m_rect.width - innerCircleReduction,
        m_rect.height - innerCircleReduction);

    const double theta = (M_PI / 180.f) * (90 + m_minAngle + (m_value - m_minValue) / (m_maxValue - m_minValue) * (m_maxAngle - m_minAngle));
    double dx = cos(theta);
    double dy = sin(theta);

    const int cx = m_rect.x + parentRect.x + (m_rect.width / 2);
    const int cy = m_rect.y + parentRect.y + (m_rect.height / 2);
    wxPen fillPen = wxPen(m_fillColour, 3);
    fillPen.SetCap(wxCAP_PROJECTING);
    gc->SetPen(fillPen);
    wxGraphicsPath fillPath = gc->CreatePath();
    fillPath.AddArc(cx, cy, m_rect.width / 2 - 2, (M_PI / 180.f) * (90 + m_minAngle), theta, true);
    gc->StrokePath(fillPath);
  }

  void Dial::onChange() {
    std::scoped_lock<std::mutex> lock(core::Project::get().dataMutex);

    core::attribute_t attribute = m_parent->getAttribute(m_attributeIndex);
    std::visit(ArithmeticToAttribute{m_value}, attribute);
  }

  OptionBox::OptionBox(CanvasNode* parent, const wxRect& rect, size_t attributeIndex, int maxOptionsPerRow, const std::vector<Option>& options)
    : CanvasControl(parent, rect, attributeIndex)
    , m_options(options)
    , m_totalOptions(options.size())
    , m_maxOptionsPerRow(maxOptionsPerRow)
    , m_rows((m_totalOptions - 1) / m_maxOptionsPerRow + 1)
    , m_currentValue(std::visit(AttributeToArithmetic<int>{}, parent->getAttribute(attributeIndex)))
  {

    size_t i = 0;
    for (auto& option : m_options) {
      wxRect& optionRect = option.rect;

      int optionsInRow = maxOptionsPerRow;

      // for final row where amount of options may be less then the maximum per row
      if (i >= m_totalOptions / m_maxOptionsPerRow * m_maxOptionsPerRow) {
        optionsInRow = m_totalOptions - (m_totalOptions / m_maxOptionsPerRow * m_maxOptionsPerRow);
      }
      optionRect.width = m_rect.width / optionsInRow;
      optionRect.x = (i % optionsInRow) * optionRect.width;
      optionRect.height = m_rect.height / m_rows;
      optionRect.y = (i / maxOptionsPerRow) * optionRect.height;

      i++;
    }
  }

  void OptionBox::onMouseDown(const wxPoint& pos) {
    int i = 0;
    for (auto& option : m_options) {
      const wxRect& rect = option.rect;
      if (wxRect(m_rect.x + rect.x, m_rect.y + rect.y, rect.width, rect.height)
          .Contains(pos)) 
      {
        m_currentValue = i;
        onChange();
      }
      i++;
    }
  }

  void OptionBox::onUpdate(wxGraphicsContext *gc, const wxRect &parentRect) const {
    const int roundness = 2;

    gc->SetPen(*wxLIGHT_GREY_PEN);
    gc->SetBrush(*wxLIGHT_GREY_BRUSH);
    gc->DrawRoundedRectangle(
        m_rect.x + parentRect.x,
        m_rect.y + parentRect.y,
        m_rect.width, m_rect.height, roundness);

    gc->SetPen(*wxGREY_PEN);
    gc->SetBrush(*wxGREY_BRUSH);
    const Option& selectedOption = m_options.at(m_currentValue);

    const wxRect& rect = selectedOption.rect;
    gc->DrawRoundedRectangle(
        rect.x + m_rect.x + parentRect.x,
        rect.y + m_rect.y + parentRect.y,
        rect.width, rect.height, roundness);

    for (auto& option : m_options) {
      const wxRect& rect = option.rect;
      const wxBitmap& image = option.image;
      const int edgePadding = 4;
      gc->DrawBitmap(image,
          rect.x + m_rect.x + parentRect.x + edgePadding,
          rect.y + m_rect.y + parentRect.y + edgePadding,
          rect.width - (edgePadding << 1), rect.height - (edgePadding << 1));
    }
  }

  void OptionBox::onChange() {
    std::scoped_lock<std::mutex> lock(core::Project::get().dataMutex);

    core::attribute_t attribute = m_parent->getAttribute(m_attributeIndex);
    std::visit(ArithmeticToAttribute{m_currentValue}, attribute);
  }

} // namespace gui

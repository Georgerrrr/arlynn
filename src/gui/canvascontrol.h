#pragma once 

#include <memory>
#include <filesystem>

#include <wx/wx.h>
#include <wx/graphics.h>

#include "configure.h"

namespace gui {

  class CanvasNode;

  class CanvasControl {
    public:
    CanvasControl(CanvasNode* parent, const wxRect& rect, size_t attributeIndex);
    virtual ~CanvasControl() = default;

    virtual void onMouseDown(const wxPoint& position) {}
    virtual void onMouseMove(const wxPoint& position) {}
    virtual void onMouseUp(const wxPoint& position) {}
    virtual void onMouseScroll(const int rotation) {} 
    virtual void onKeyPress(const char key) {}

    virtual void onUpdate(wxGraphicsContext* gc, const wxRect& parentRect) const {}

    virtual void setLoseFocus() { m_focus = false; }

    // some controls (such as text box only need to be clicked on and can be edited after. The dial cannot.
    virtual constexpr bool loseFocusOnMouseUp() const { return true; }

    bool mouseCollide(const wxRect& parentRect, const wxPoint& mousePos) {
      if (wxRect(
            m_rect.x + parentRect.x,
            m_rect.y + parentRect.y,
            m_rect.width, m_rect.height).Contains(mousePos)) {
        m_focus = true;
        return true;
      }
      return false;
    }

    const wxRect& getRect() const { return m_rect; }
    const bool getFocus() const { return m_focus; }

    protected:
    bool m_focus;
    size_t m_attributeIndex;
    CanvasNode* m_parent;
    wxRect m_rect;
  };

  class TextBox : public CanvasControl {
    public:
    TextBox(CanvasNode* parent, const wxRect& rect, size_t attributeIndex, const wxString& value);
    constexpr bool loseFocusOnMouseUp() const override { return false; }
    void onKeyPress(const char key) override;

    void onUpdate(wxGraphicsContext* gc, const wxRect& parentRect) const override;

    void setLoseFocus() override;
    private:
    static const wxFont m_font;
    wxString m_value;
  };

  class Dial : public CanvasControl {
    public:
    Dial(CanvasNode* parent, const wxRect& rect, size_t attributeIndex, const wxColour& fillColour, double minValue, double defaultValue, double maxValue, double microChange);
    constexpr bool loseFocusOnMouseUp() const override { return true; }

    void onMouseDown(const wxPoint& pos) override;
    void onMouseMove(const wxPoint& pos) override;

    void onMouseScroll(const int rotation) override;

    void onUpdate(wxGraphicsContext* gc, const wxRect& parentRect) const override;
    private:
    double m_value;
    wxColour m_fillColour;

    wxPoint m_mousePos;

    void onChange();
    
    const double m_minValue;
    const double m_maxValue;
    const double m_microChange;
    const int m_minAngle = 40;
    const int m_maxAngle = 320;
  };

  class OptionBox : public CanvasControl {
    public:
    struct Option {
      Option() = default;
      Option(const std::filesystem::path& path) 
        : image(((RESOURCE_PATH) / path).c_str(), wxBITMAP_TYPE_BMP)
      {
      }
      wxRect rect;
      wxBitmap image;
    };

    OptionBox(CanvasNode* parent, const wxRect& rect, size_t attributeIndex, int maxOptionsPerRow, int defaultvalue, const std::vector<Option>& options);

    void onMouseDown(const wxPoint& pos) override;

    void onUpdate(wxGraphicsContext* gc, const wxRect& parentRect) const override;

    private:
    void onChange();

    const int m_totalOptions;
    const int m_maxOptionsPerRow;
    const int m_rows;

    int m_currentValue;

    std::vector<Option> m_options;

  };

} // namespace gui

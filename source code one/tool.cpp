#include "tool.h"

Tool::Tool()
    : m_type(ToolType::None),
      m_color(Qt::black),
      m_thickness(1),
      m_opacity(1.0f),
      m_fillEnabled(false),
      m_angleValue(0) {}

Tool::Tool(ToolType type, const QColor& color, int thickness, float opacity)
    : m_type(type),
      m_color(color),
      m_thickness(thickness),
      m_opacity(opacity),
      m_fillEnabled(false),
      m_angleValue(0) {}

ToolType Tool::type() const { return m_type; }
void Tool::setType(ToolType type) { m_type = type; }

QColor Tool::color() const { return m_color; }
void Tool::setColor(const QColor& color) { m_color = color; }

int Tool::thickness() const { return m_thickness; }
void Tool::setThickness(int value) { m_thickness = value; }

float Tool::opacity() const { return m_opacity; }
void Tool::setOpacity(float value) { m_opacity = value; }

bool Tool::fillEnabled() const { return m_fillEnabled; }
void Tool::setFillEnabled(bool enabled) { m_fillEnabled = enabled; }

int Tool::angleValue() const { return m_angleValue; }
void Tool::setAngleValue(int degrees) { m_angleValue = degrees; }

QString Tool::name() const {
    switch (m_type) {
        case ToolType::NewFile: return "New File";
        case ToolType::OpenFile: return "Open File";
        case ToolType::SaveFile: return "Save File";
        case ToolType::ColorPicker: return "Color Picker";
        case ToolType::Bucket: return "Bucket";
        case ToolType::Cut: return "Cut";
        case ToolType::Copy: return "Copy";
        case ToolType::Paste: return "Paste";
        case ToolType::Ellipse: return "Ellipse";
        case ToolType::Eraser: return "Eraser";
        case ToolType::Eyedropper: return "Eyedropper";
        case ToolType::FillToggle: return "Fill Toggle";
        case ToolType::FitToScreen: return "Fit to Screen";
        case ToolType::FlipHorizontal: return "Flip Horizontal";
        case ToolType::FlipVertical: return "Flip Vertical";
        case ToolType::Line: return "Line";
        case ToolType::Pencil: return "Pencil";
        case ToolType::Rectangle: return "Rectangle";
        case ToolType::Redo: return "Redo";
        case ToolType::Undo: return "Undo";
        case ToolType::Rotate: return "Rotate";
        case ToolType::Select: return "Select";
        case ToolType::Spray: return "Spray";
        case ToolType::Text: return "Text";
        case ToolType::ZoomIn: return "Zoom In";
        case ToolType::ZoomOut: return "Zoom Out";
        default: return "None";
    }
}

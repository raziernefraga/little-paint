#include "tool.h"

Tool::Tool(ToolType type, const QColor &outlineColor, const QColor &fillColor,
           int thickness, double opacity)
    : m_type(type),
      m_outlineColor(outlineColor),
      m_fillColor(fillColor),
      m_thickness(thickness),
      m_opacity(opacity),
      m_fillEnabled(false),
      m_font(QFont("Arial", 14)),
      m_textColor(Qt::black)
{}

ToolType Tool::type() const { return m_type; }
void Tool::setType(ToolType type) { m_type = type; }

QColor Tool::outlineColor() const { return m_outlineColor; }
void Tool::setOutlineColor(const QColor &color) { m_outlineColor = color; }

QColor Tool::fillColor() const { return m_fillColor; }
void Tool::setFillColor(const QColor &color) { m_fillColor = color; }

int Tool::thickness() const { return m_thickness; }
void Tool::setThickness(int value) { m_thickness = value; }

double Tool::opacity() const { return m_opacity; }
void Tool::setOpacity(double value) { m_opacity = value; }

bool Tool::fillEnabled() const { return m_fillEnabled; }
void Tool::setFillEnabled(bool enabled) { m_fillEnabled = enabled; }

QFont Tool::font() const { return m_font; }
void Tool::setFont(const QFont &font) { m_font = font; }

QColor Tool::textColor() const { return m_textColor; }
void Tool::setTextColor(const QColor &color) { m_textColor = color; }

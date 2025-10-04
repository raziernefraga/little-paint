#include <QPainterPath>
#include "tool.h"

Tool::Tool()
    : toolType(ToolType::None),
      outline(Qt::black),
      fill(Qt::white),
      filled(false),
      lineThickness(2),
      alpha(1.0f),
      textFont("Arial", 12)
{}

// Tipo
ToolType Tool::type() const {
    return toolType;
}

void Tool::setType(ToolType newType) {
    toolType = newType;
}

// Cores
QColor Tool::outlineColor() const {
    return outline;
}

void Tool::setOutlineColor(const QColor &color) {
    outline = color;
}

QColor Tool::fillColor() const {
    return fill;
}

void Tool::setFillColor(const QColor &color) {
    fill = color;
}

bool Tool::fillEnabled() const {
    return filled;
}

void Tool::setFillEnabled(bool enabled) {
    filled = enabled;
}

// Espessura e opacidade
int Tool::thickness() const {
    return lineThickness;
}

void Tool::setThickness(int value) {
    lineThickness = value;
}

float Tool::opacity() const {
    return alpha;
}

void Tool::setOpacity(float value) {
    alpha = value;
}

// Fonte
QFont Tool::font() const {
    return textFont;
}

void Tool::setFont(const QFont &f) {
    textFont = f;
}

// Aplicação no canvas
void Tool::apply(QPainter &painter, const QPoint &start, const QPoint &end) const {
    painter.setPen(QPen(outline, lineThickness));
    painter.setOpacity(alpha);

    if (filled) {
        painter.setBrush(QBrush(fill));
    } else {
        painter.setBrush(Qt::NoBrush);
    }

    switch (toolType) {
        case ToolType::Line:
            painter.drawLine(start, end);
            break;
        case ToolType::Rectangle:
            painter.drawRect(QRect(start, end));
            break;
        case ToolType::Ellipse:
            painter.drawEllipse(QRect(start, end));
            break;
        case ToolType::Triangle: {
            QPolygon triangle;
            triangle << start
                     << QPoint(end.x(), start.y())
                     << end;
            painter.drawPolygon(triangle);
            break;
        }
        case ToolType::Curve: {
            QPainterPath path;
            path.moveTo(start);
            path.quadTo(QPoint((start + end) / 2), end);
            painter.drawPath(path);
            break;
        }
        case ToolType::Text: {
            painter.setFont(textFont);
            painter.drawText(QRect(start, end), Qt::AlignLeft | Qt::AlignTop, "Texto");
            break;
        }
        default:
            break;
    }
}

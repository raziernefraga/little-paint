#include <QPainterPath>
#include <QRandomGenerator>
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
ToolType Tool::type() const { return toolType; }
void Tool::setType(ToolType newType) { toolType = newType; }

// Cores
QColor Tool::outlineColor() const { return outline; }
void Tool::setOutlineColor(const QColor &color) { outline = color; }

QColor Tool::fillColor() const { return fill; }
void Tool::setFillColor(const QColor &color) { fill = color; }

bool Tool::fillEnabled() const { return filled; }
void Tool::setFillEnabled(bool enabled) { filled = enabled; }

// Espessura e opacidade
int Tool::thickness() const { return lineThickness; }
void Tool::setThickness(int value) { lineThickness = value; }

float Tool::opacity() const { return alpha; }
void Tool::setOpacity(float value) { alpha = value; }

// Fonte
QFont Tool::font() const { return textFont; }
void Tool::setFont(const QFont &f) { textFont = f; }

// Aplicação no canvas
void Tool::apply(QPainter &painter, const QPoint &start, const QPoint &end) const {
    painter.setPen(QPen(outline, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setOpacity(alpha);

    if (filled) {
        painter.setBrush(QBrush(fill));
    } else {
        painter.setBrush(Qt::NoBrush);
    }

    QRect rect(start, end);

    switch (toolType) {
        case ToolType::Line:
            painter.drawLine(start, end);
            break;

        case ToolType::Rectangle:
            if (filled) painter.fillRect(rect, fill);
            painter.drawRect(rect);
            break;

        case ToolType::Ellipse:
            painter.drawEllipse(rect);
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
            QPoint control = (start + end) / 2 + QPoint(0, -40);
            path.moveTo(start);
            path.quadTo(control, end);
            painter.drawPath(path);
            break;
        }

        case ToolType::Text:
            painter.setFont(textFont);
            painter.drawText(rect, Qt::AlignLeft | Qt::AlignTop, "Texto");
            break;

        case ToolType::Pencil:
        case ToolType::Brush: {
            QPen pen(outline, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);
            painter.drawLine(start, end);
            break;
        }

        case ToolType::Spray: {
            int radius = lineThickness * 2;
            int density = 100;
            for (int i = 0; i < density; ++i) {
                int dx = QRandomGenerator::global()->bounded(-radius, radius);
                int dy = QRandomGenerator::global()->bounded(-radius, radius);
                QPoint point(start.x() + dx, start.y() + dy);
                if (QLineF(start, point).length() <= radius) {
                    painter.drawPoint(point);
                }
            }
            break;
        }

        case ToolType::Eyedropper:
            // Eyedropper não desenha — tratado no CanvasWidget
            break;

        case ToolType::Eraser: {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            QPen eraserPen(Qt::transparent, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(eraserPen);
            painter.drawLine(start, end);
            break;
        }

        default:
            break;
    }
}

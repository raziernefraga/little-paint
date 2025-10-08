#include "tool.h"
#include <QPainterPath>
#include <QRandomGenerator>

Tool::Tool()
    : toolType(ToolType::None),
      primaryColor(Qt::black),
      outlineColor_(Qt::black),
      fillColor_(Qt::white),
      fillEnabled_(false),
      thickness_(2),
      opacity_(1.0f),
      text_("Texto"),
      font_(QFont("Arial", 12)),
      bold_(false),
      italic_(false)
{}

// Tipo
ToolType Tool::type() const { return toolType; }
void Tool::setType(ToolType newType) { toolType = newType; }

// Cor principal
void Tool::setColor(const QColor &color) { primaryColor = color; }
QColor Tool::color() const { return primaryColor; }

// Contorno e preenchimento
void Tool::setOutlineColor(const QColor &color) { outlineColor_ = color; }
QColor Tool::outlineColor() const { return outlineColor_; }

void Tool::setFillColor(const QColor &color) { fillColor_ = color; }
QColor Tool::fillColor() const { return fillColor_; }

void Tool::setFillEnabled(bool enabled) { fillEnabled_ = enabled; }
bool Tool::fillEnabled() const { return fillEnabled_; }

// Espessura e opacidade
void Tool::setThickness(int value) { thickness_ = value; }
int Tool::thickness() const { return thickness_; }

void Tool::setOpacity(float value) { opacity_ = value; }
float Tool::opacity() const { return opacity_; }

// Texto
void Tool::setText(const QString &text) { text_ = text; }
QString Tool::text() const { return text_; }

void Tool::setFont(const QFont &f) { font_ = f; }
QFont Tool::font() const { return font_; }

void Tool::setBold(bool enabled) { bold_ = enabled; }
bool Tool::bold() const { return bold_; }

void Tool::setItalic(bool enabled) { italic_ = enabled; }
bool Tool::italic() const { return italic_; }

// Aplicação automática (usado em modos como linha, triângulo, etc.)
void Tool::apply(QPainter &painter, const QPoint &start, const QPoint &end) const {
    if (toolType == ToolType::Eraser) {
        QPen eraserPen(Qt::transparent, thickness_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setPen(eraserPen);
        painter.drawLine(start, end);
        return;
    }

    painter.setPen(QPen(outlineColor_, thickness_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setOpacity(opacity_);
    painter.setBrush(fillEnabled_ ? QBrush(fillColor_) : Qt::NoBrush);

    QRect rect(start, end);

    switch (toolType) {
        case ToolType::Line:
            painter.drawLine(start, end);
            break;

        case ToolType::Rectangle:
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

        case ToolType::Text: {
            QFont styledFont = font_;
            styledFont.setBold(bold_);
            styledFont.setItalic(italic_);
            painter.setFont(styledFont);
            painter.drawText(rect, Qt::AlignLeft | Qt::AlignTop, text_);
            break;
        }

        case ToolType::Pencil:
        case ToolType::Brush: {
            QPen pen(primaryColor, thickness_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);
            painter.drawLine(start, end);
            break;
        }

        case ToolType::Spray: {
            int radius = thickness_ * 2;
            int density = thickness_ * 5;

            for (int i = 0; i < density; ++i) {
                int dx = QRandomGenerator::global()->bounded(-radius, radius);
                int dy = QRandomGenerator::global()->bounded(-radius, radius);
                QPoint point(end.x() + dx, end.y() + dy);
                if (QLineF(end, point).length() <= radius) {
                    painter.drawPoint(point);
                }
            }
            break;
        }

        case ToolType::Eyedropper:
        case ToolType::Select:
        case ToolType::Bucket:
        case ToolType::None:
            break;
    }
}

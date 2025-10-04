#ifndef TOOL_H
#define TOOL_H

#include <QColor>
#include <QFont>
#include <QPoint>
#include <QPainter>

enum class ToolType {
    None,
    Pencil,
    Brush,
    Line,
    Rectangle,
    Ellipse,
    Triangle,
    Curve,
    Bucket,
    Spray,
    Eyedropper,
    Text,
    Select,
    Eraser,
};

class Tool {
public:
    Tool();

    // Tipo
    ToolType type() const;
    void setType(ToolType newType);

    // Estilo
    QColor outlineColor() const;
    void setOutlineColor(const QColor &color);

    QColor fillColor() const;
    void setFillColor(const QColor &color);

    bool fillEnabled() const;
    void setFillEnabled(bool enabled);

    int thickness() const;
    void setThickness(int value);

    float opacity() const;
    void setOpacity(float value);

    QFont font() const;
    void setFont(const QFont &f);

    // Aplicação
    void apply(QPainter &painter, const QPoint &start, const QPoint &end) const;

private:
    ToolType toolType;
    QColor outline;
    QColor fill;
    bool filled;
    int lineThickness;
    float alpha;
    QFont textFont;
};

#endif // TOOL_H

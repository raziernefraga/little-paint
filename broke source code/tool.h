#ifndef TOOL_H
#define TOOL_H

#include <QColor>
#include <QFont>

enum class ToolType {
    None,
    Pencil,
    Eraser,
    Bucket,
    Line,
    Rectangle,
    Ellipse,
    Spray,
    Text,
    Eyedropper,
    Select,
    Cut,
    Copy,
    Paste,
    Undo,
    Redo,
    FlipHorizontal,
    FlipVertical,
    Rotate,
    ZoomIn,
    ZoomOut,
    FitToScreen
};

class Tool {
public:
    Tool(ToolType type = ToolType::None,
         const QColor &outlineColor = Qt::black,
         const QColor &fillColor = Qt::transparent,
         int thickness = 1,
         double opacity = 1.0);

    ToolType type() const;
    void setType(ToolType type);

    QColor outlineColor() const;
    void setOutlineColor(const QColor &color);

    QColor fillColor() const;
    void setFillColor(const QColor &color);

    int thickness() const;
    void setThickness(int value);

    double opacity() const;
    void setOpacity(double value);

    bool fillEnabled() const;
    void setFillEnabled(bool enabled);

    // Texto
    QFont font() const;
    void setFont(const QFont &font);

    QColor textColor() const;
    void setTextColor(const QColor &color);

private:
    ToolType m_type;
    QColor m_outlineColor;
    QColor m_fillColor;
    int m_thickness;
    double m_opacity;
    bool m_fillEnabled;

    // Texto
    QFont m_font;
    QColor m_textColor;
};

#endif // TOOL_H

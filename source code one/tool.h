#ifndef TOOL_H
#define TOOL_H

#include <QString>
#include <QColor>

enum class ToolType {
    None,
    NewFile,
    OpenFile,
    SaveFile,
    ColorPicker,
    Bucket,
    Cut,
    Copy,
    Paste,
    Ellipse,
    Eraser,
    Eyedropper,
    FillToggle,
    FitToScreen,
    FlipHorizontal,
    FlipVertical,
    Line,
    Pencil,
    Rectangle,
    Redo,
    Undo,
    Rotate,
    Select,
    Spray,
    Text,
    ZoomIn,
    ZoomOut
};

class Tool {
public:
    Tool();
    Tool(ToolType type, const QColor& color, int thickness = 1, float opacity = 1.0f);

    ToolType type() const;
    void setType(ToolType type);

    QColor color() const;
    void setColor(const QColor& color);

    int thickness() const;
    void setThickness(int value);

    float opacity() const;
    void setOpacity(float value);

    bool fillEnabled() const;
    void setFillEnabled(bool enabled);

    int angleValue() const;
    void setAngleValue(int degrees);

    QString name() const;

private:
    ToolType m_type;
    QColor m_color;
    int m_thickness;
    float m_opacity;
    bool m_fillEnabled;
    int m_angleValue;
};

#endif // TOOL_H

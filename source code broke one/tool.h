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

    // Cor principal
    void setColor(const QColor &color);
    QColor color() const;

    // Contorno e preenchimento
    void setOutlineColor(const QColor &color);
    QColor outlineColor() const;

    void setFillColor(const QColor &color);
    QColor fillColor() const;

    // Preenchimento ativado
    void setFillEnabled(bool enabled);
    bool fillEnabled() const;

    // Espessura e opacidade
    void setThickness(int value);
    int thickness() const;

    void setOpacity(float value);
    float opacity() const;

    // Texto
    void setText(const QString &text);
    QString text() const;

    void setFont(const QFont &font);
    QFont font() const;

    void setBold(bool enabled);
    bool bold() const;

    void setItalic(bool enabled);
    bool italic() const;

    // Aplicação automática (usado em modos como linha, triângulo, etc.)
    void apply(QPainter &painter, const QPoint &start, const QPoint &end) const;

private:
    ToolType toolType;

    QColor primaryColor;
    QColor outlineColor_;
    QColor fillColor_;
    bool fillEnabled_;        // ✅ restaurado

    int thickness_;
    float opacity_;

    QString text_;
    QFont font_;
    bool bold_;
    bool italic_;
};

#endif // TOOL_H

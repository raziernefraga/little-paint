#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QPoint>
#include <QColor>
#include <QStack>

#include "tool.h"

class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    // Ferramentas
    void setActiveTool(const Tool &tool);
    Tool activeTool() const;
    QSize canvasSize() const;

    void setColor(const QColor &color);
    void setOutlineColor(const QColor &color);
    void setFillColor(const QColor &color);
    void setThickness(int value);
    void setOpacity(float value);

    // Canvas
    void clearCanvas();
    void resizeCanvas(int width, int height);
    void openImage(const QString &path);
    void saveImage(const QString &path);
    void exportImage(const QString &path);
    void setExportTransparency(bool enabled);

    // Edição
    void undo();
    void redo();

    // Visualização
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void toggleGrid();

    // Seleção
    void copySelection();
    void cutSelection();
    void pasteSelection();
    void applySelection();
    void flipSelectionHorizontal();
    void flipSelectionVertical();
    void rotateSelection(int angle);

signals:
    void colorPicked(const QColor &color);
    void outlineColorPicked(const QColor &color);
    void fillColorPicked(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage canvasImage;
    Tool currentTool;

    // Seleção
    QRect selectionRect;
    QImage selectionImage;
    bool selectionActive;
    bool movingSelection;
    bool resizingSelection;
    QPoint dragStartPos;
    QRect initialSelectionRect;
    int resizeHandleIndex;

    // Visualização
    float zoomFactor;
    bool showGrid;

    // Ferramentas de desenho
    QPoint lastPoint;
    QRect tempRect;

    // Métodos auxiliares
    QRect normalizedRect(const QPoint &p1, const QPoint &p2) const;
    void drawSelectionFeedback(QPainter &painter);
    int hitTestHandles(const QPoint &pos) const;

    // Métodos de desenho
    void drawAt(const QPoint &pos);
    void drawRectangle(const QRect &rect);
    void drawEllipse(const QRect &rect);
    void drawText(const QRect &rect);
    void pickColor(const QPoint &pos);
};

#endif // CANVASWIDGET_H

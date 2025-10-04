#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QStack>
#include "tool.h"

class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    void setActiveTool(const Tool &tool);
    void undo();
    void redo();
    void flipHorizontal();
    void flipVertical();
    void rotateCanvas(int angle);
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void clearCanvas();
    void openImage(const QString &path);
    void saveImage(const QString &path);
    void resizeCanvas(int width, int height); // ✅ Adicionado

    // Seleção
    void clearSelection();
    void copySelection();
    void cutSelection();
    void pasteSelection();
    void applySelection();
    void flipSelectionHorizontal();
    void flipSelectionVertical();
    void rotateSelection(int angle);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage m_canvas;
    Tool m_activeTool;
    float m_zoomFactor;

    QPoint m_startPoint;
    QPoint m_endPoint;
    bool m_drawing;

    QStack<QImage> m_undoStack;
    QStack<QImage> m_redoStack;

    // Seleção
    bool m_hasSelection;
    QRect m_selectionRect;
    QImage m_selectionImage;
    QPoint m_selectionOffset;
    bool m_movingSelection;

    // Métodos auxiliares
    void pushUndo();
    void applyFloodFill(const QPoint &pt, const QColor &newColor);
    void applySpray(const QPoint &pos);
    void applyEyedropper(const QPoint &pos);
    void drawSelectionOutline(QPainter &painter);
};

#endif // CANVASWIDGET_H

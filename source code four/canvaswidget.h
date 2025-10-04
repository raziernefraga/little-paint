#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QRect>
#include "tool.h"

class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    // Zoom e redimensionamento
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void resizeCanvas(int width, int height);

    // Ferramenta ativa
    Tool activeTool() const;
    void setActiveTool(const Tool &newTool);

    // Imagem
    void clearCanvas();
    void undo();
    void redo();
    void openImage(const QString &path);
    void saveImage(const QString &path);

    // Seleção
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
    void drawPreviewShape(QPainter &painter);

    QImage canvasImage;
    float zoomFactor;

    Tool tool;
    bool isDrawing;
    bool previewActive;
    bool selectionActive;

    QPoint lastPoint;
    QPoint previewStart;
    QPoint previewEnd;
    QRect selectionRect;
};

#endif // CANVASWIDGET_H

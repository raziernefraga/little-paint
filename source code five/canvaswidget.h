#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QVector>
#include <QString>
#include <QColor>
#include "tool.h"
#include "UndoStack.h"

class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    // Zoom e redimensionamento
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void resizeCanvas(int width, int height);
    void setZoomFactor(double factor);
    void toggleGrid();

    // Ferramenta ativa
    Tool activeTool() const;
    void setActiveTool(const Tool &newTool);

    // Métodos auxiliares para MainWindow
    Tool getTool() const;
    void setColor(const QColor &color);
    void setThickness(int value);

    // Imagem
    void clearCanvas();
    void undo();
    void redo();
    void openImage(const QString &path);
    void saveImage(const QString &path);
    bool exportImage(const QString &path, const char *format = "png");

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

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void drawPreviewShape(QPainter &painter);

    // Estado da seleção
    QRect selectionRect;
    QImage selectionImage;
    bool selectionActive = false;

    // Camadas e imagem principal
    QImage canvasImage;
    QImage backgroundLayer;
    QImage drawingLayer;
    QImage selectionLayer;

    // Histórico visual
    QVector<QImage> historyThumbnails;
    UndoStack undoStack;

    // Ferramenta e desenho
    Tool tool;
    bool isDrawing = false;
    bool previewActive = false;

    // Zoom e grade
    float zoomFactor = 1.0f;
    bool showGrid = false;

    // Pontos de controle
    QPoint lastPoint;
    QPoint previewStart;
    QPoint previewEnd;
};

#endif // CANVASWIDGET_H

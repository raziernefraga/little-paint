#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QColor>
#include <QStack>
#include <QFont>
#include "tool.h"

class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    // Ferramenta ativa
    void setActiveTool(const Tool &tool);
    Tool activeTool() const;

    // Ações de arquivo
    void clearCanvas();
    void openImage(const QString &path);
    void saveImage(const QString &path);

    // Edição
    void copySelection();
    void cutSelection();
    void pasteClipboard();

    // Transformações
    void flipHorizontal();
    void flipVertical();
    void rotateCanvas(int angle);

    // Zoom e visualização
    void zoomIn();
    void zoomOut();
    void fitToScreen();

    // Histórico
    void undo();
    void redo();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Tool m_activeTool;
    QImage m_canvas;
    QImage m_backup;
    QStack<QImage> m_undoStack;
    QStack<QImage> m_redoStack;
    QPoint m_startPoint;
    QPoint m_endPoint;
    bool m_drawing;
    QRect m_selection;
    QImage m_clipboard;
    float m_zoomFactor;
    QFont m_textFont;

    // Desenho de formas e efeitos
    void drawShape();
    void applySpray(const QPoint &pos);
    void applyEyedropper(const QPoint &pos);
    void pushUndo();
};

#endif // CANVASWIDGET_H

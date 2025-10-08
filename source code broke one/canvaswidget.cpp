#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QDebug>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
      selectionActive(false),
      movingSelection(false),
      resizingSelection(false),
      resizeHandleIndex(-1),
      zoomFactor(1.0f),
      showGrid(false)
{
    setAttribute(Qt::WA_StaticContents);
    canvasImage = QImage(800, 600, QImage::Format_ARGB32_Premultiplied);
    canvasImage.fill(Qt::white);
}

void CanvasWidget::setActiveTool(const Tool &tool) {
    currentTool = tool;
}

Tool CanvasWidget::activeTool() const {
    return currentTool;
}

QSize CanvasWidget::canvasSize() const {
    return canvasImage.size();
}

void CanvasWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Aplica zoom
    painter.save();
    painter.scale(zoomFactor, zoomFactor);
    painter.drawImage(QPoint(0, 0), canvasImage);
    painter.restore();

    // Desenha grade se ativada
    if (showGrid) {
        const int gridSize = 20;
        QPen gridPen(QColor(220, 220, 220), 1);
        painter.setPen(gridPen);

        int w = width();
        int h = height();
        for (int x = 0; x < w; x += gridSize)
            painter.drawLine(x, 0, x, h);
        for (int y = 0; y < h; y += gridSize)
            painter.drawLine(0, y, w, y);
    }

    // Feedback visual da seleção
    if (selectionActive && !selectionRect.isNull()) {
        drawSelectionFeedback(painter);
    }
}
void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos() / zoomFactor;

    if (currentTool.type() == ToolType::Select) {
        resizeHandleIndex = hitTestHandles(pos);
        if (resizeHandleIndex != -1 && selectionActive) {
            resizingSelection = true;
            dragStartPos = pos;
            initialSelectionRect = selectionRect;
            return;
        }

        if (selectionActive && selectionRect.contains(pos)) {
            movingSelection = true;
            dragStartPos = pos;
            initialSelectionRect = selectionRect;
            return;
        }

        selectionActive = true;
        selectionRect = QRect(pos, pos);
        update();
    }
        else {
        lastPoint = event->pos() / zoomFactor;

        if (currentTool.type() == ToolType::Eyedropper) {
            pickColor(lastPoint);
        } else if (currentTool.type() == ToolType::Text ||
                   currentTool.type() == ToolType::Rectangle ||
                   currentTool.type() == ToolType::Ellipse) {
            tempRect = QRect(lastPoint, lastPoint);
        } else {
            drawAt(lastPoint);
        }
    }

}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->pos() / zoomFactor;

    if (currentTool.type() == ToolType::Select) {
        if (resizingSelection) {
            QRect newRect = initialSelectionRect;
            QPoint delta = pos - dragStartPos;

            switch (resizeHandleIndex) {
            case 0: newRect.setTopLeft(initialSelectionRect.topLeft() + delta); break;
            case 1: newRect.setTopRight(initialSelectionRect.topRight() + delta); break;
            case 2: newRect.setBottomLeft(initialSelectionRect.bottomLeft() + delta); break;
            case 3: newRect.setBottomRight(initialSelectionRect.bottomRight() + delta); break;
            }

            selectionRect = newRect.normalized();
            update();
        }
        else if (movingSelection) {
            QPoint delta = pos - dragStartPos;
            selectionRect = initialSelectionRect.translated(delta);
            update();
        }
        else if (selectionActive && !selectionRect.isNull()) {
            selectionRect = normalizedRect(selectionRect.topLeft(), pos);
            update();
        }
    }
        else {
        QPoint currentPoint = event->pos() / zoomFactor;

        if (currentTool.type() == ToolType::Pencil ||
            currentTool.type() == ToolType::Brush) {
            drawAt(currentPoint);
            lastPoint = currentPoint;
        } else if (currentTool.type() == ToolType::Rectangle ||
                   currentTool.type() == ToolType::Ellipse ||
                   currentTool.type() == ToolType::Text) {
            tempRect = QRect(lastPoint, currentPoint).normalized();
            update();
        }
    }

}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);

    if (currentTool.type() == ToolType::Select) {
        resizingSelection = false;
        movingSelection = false;
        resizeHandleIndex = -1;
        update();
    }
        else {
        QPoint releasePoint = event->pos() / zoomFactor;

        if (currentTool.type() == ToolType::Rectangle) {
            drawRectangle(QRect(lastPoint, releasePoint).normalized());
        } else if (currentTool.type() == ToolType::Ellipse) {
            drawEllipse(QRect(lastPoint, releasePoint).normalized());
        } else if (currentTool.type() == ToolType::Text) {
            drawText(QRect(lastPoint, releasePoint).normalized());
        }
    }

}
// =========================
// Métodos auxiliares
// =========================

QRect CanvasWidget::normalizedRect(const QPoint &p1, const QPoint &p2) const {
    return QRect(p1, p2).normalized();
}

int CanvasWidget::hitTestHandles(const QPoint &pos) const {
    if (!selectionActive || selectionRect.isNull())
        return -1;

    const int handleSize = 6;
    QVector<QPoint> handles = {
        selectionRect.topLeft(),
        selectionRect.topRight(),
        selectionRect.bottomLeft(),
        selectionRect.bottomRight()
    };

    for (int i = 0; i < handles.size(); ++i) {
        QRect handleRect(handles[i].x() - handleSize/2,
                         handles[i].y() - handleSize/2,
                         handleSize, handleSize);
        if (handleRect.contains(pos))
            return i;
    }
    return -1;
}

void CanvasWidget::drawSelectionFeedback(QPainter &painter) {
    if (selectionRect.isNull())
        return;

    painter.save();
    painter.setOpacity(0.3);
    painter.setBrush(Qt::gray);
    painter.setPen(Qt::DashLine);
    painter.drawRect(selectionRect);

    // Desenha alças de redimensionamento
    const int handleSize = 6;
    QVector<QPoint> handles = {
        selectionRect.topLeft(),
        selectionRect.topRight(),
        selectionRect.bottomLeft(),
        selectionRect.bottomRight()
    };

    painter.setBrush(Qt::black);
    for (const QPoint &handle : handles) {
        QRect handleRect(handle.x() - handleSize / 2,
                         handle.y() - handleSize / 2,
                         handleSize, handleSize);
        painter.drawRect(handleRect);
    }

    painter.restore();
}

// =========================
// Zoom e grade
// =========================

void CanvasWidget::zoomIn() {
    zoomFactor *= 1.2f;
    update();
}

void CanvasWidget::zoomOut() {
    zoomFactor /= 1.2f;
    update();
}

void CanvasWidget::fitToScreen() {
    if (!canvasImage.isNull()) {
        QSize canvasSize = canvasImage.size();
        QSize widgetSize = size();

        float scaleX = static_cast<float>(widgetSize.width()) / canvasSize.width();
        float scaleY = static_cast<float>(widgetSize.height()) / canvasSize.height();
        zoomFactor = qMin(scaleX, scaleY);
        update();
    }
}

void CanvasWidget::toggleGrid() {
    showGrid = !showGrid;
    update();
}
// =========================
// Seleção: copiar / cortar / colar / aplicar
// =========================

void CanvasWidget::copySelection() {
    if (selectionActive && !selectionRect.isNull()) {
        selectionImage = canvasImage.copy(selectionRect);
    }
}

void CanvasWidget::cutSelection() {
    if (selectionActive && !selectionRect.isNull()) {
        selectionImage = canvasImage.copy(selectionRect);
        QPainter p(&canvasImage);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(selectionRect, Qt::transparent);
        p.end();
        update();
    }
}

void CanvasWidget::pasteSelection() {
    if (!selectionImage.isNull()) {
        QPainter p(&canvasImage);
        p.drawImage(selectionRect.topLeft(), selectionImage);
        p.end();
        update();
    }
}

void CanvasWidget::applySelection() {
    selectionActive = false;
    selectionRect = QRect();
    selectionImage = QImage();
    update();
}

// =========================
// Seleção: flip horizontal / vertical
// =========================

void CanvasWidget::flipSelectionHorizontal() {
    if (selectionActive && !selectionRect.isNull()) {
        selectionImage = selectionImage.mirrored(true, false);
        QPainter p(&canvasImage);
        p.drawImage(selectionRect.topLeft(), selectionImage);
        p.end();
        update();
    } else {
        canvasImage = canvasImage.mirrored(true, false);
        update();
    }
}

void CanvasWidget::flipSelectionVertical() {
    if (selectionActive && !selectionRect.isNull()) {
        selectionImage = selectionImage.mirrored(false, true);
        QPainter p(&canvasImage);
        p.drawImage(selectionRect.topLeft(), selectionImage);
        p.end();
        update();
    } else {
        canvasImage = canvasImage.mirrored(false, true);
        update();
    }
}

// =========================
// Seleção: rotação
// =========================

void CanvasWidget::rotateSelection(int angle) {
    if (selectionActive && !selectionRect.isNull()) {
        QTransform transform;
        transform.rotate(angle);
        selectionImage = selectionImage.transformed(transform);

        QPainter p(&canvasImage);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        p.drawImage(selectionRect.topLeft(), selectionImage);
        p.end();
        update();
    } else {
        QTransform transform;
        transform.rotate(angle);
        canvasImage = canvasImage.transformed(transform);
        update();
    }
}
void CanvasWidget::setExportTransparency(bool enabled) {
    if (enabled) {
        canvasImage = canvasImage.convertToFormat(QImage::Format_ARGB32);
    } else {
        QImage opaque(canvasImage.size(), QImage::Format_RGB32);
        opaque.fill(Qt::white);
        QPainter p(&opaque);
        p.drawImage(0, 0, canvasImage);
        canvasImage = opaque;
    }
    update();
}

void CanvasWidget::saveImage(const QString &path) {
    canvasImage.save(path);
}

void CanvasWidget::exportImage(const QString &path) {
    canvasImage.save(path);
}

void CanvasWidget::undo() {
    // Implementação futura ou integração com UndoStack
}

void CanvasWidget::redo() {
    // Implementação futura ou integração com UndoStack
}

void CanvasWidget::clearCanvas() {
    canvasImage.fill(Qt::white);
    update();
}

void CanvasWidget::resizeCanvas(int width, int height) {
    QImage newImage(width, height, QImage::Format_ARGB32_Premultiplied);
    newImage.fill(Qt::white);
    QPainter p(&newImage);
    p.drawImage(0, 0, canvasImage);
    canvasImage = newImage;
    update();
}

void CanvasWidget::openImage(const QString &path) {
    QImage loaded(path);
    if (!loaded.isNull()) {
        canvasImage = loaded.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        update();
    }
}

void CanvasWidget::setColor(const QColor &color) {
    currentTool.setColor(color);
}

void CanvasWidget::setOutlineColor(const QColor &color) {
    currentTool.setOutlineColor(color);
}

void CanvasWidget::setFillColor(const QColor &color) {
    currentTool.setFillColor(color);
}

void CanvasWidget::setThickness(int value) {
    currentTool.setThickness(value);
}

void CanvasWidget::setOpacity(float value) {
    currentTool.setOpacity(value);
}
// =========================
// Métodos de desenho
// =========================

void CanvasWidget::drawAt(const QPoint &pos) {
    QPainter p(&canvasImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(currentTool.color(), currentTool.thickness(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setOpacity(currentTool.opacity());

    if (currentTool.type() == ToolType::Pencil) {
        p.drawPoint(pos);
    } else if (currentTool.type() == ToolType::Brush) {
        p.setBrush(currentTool.color());
        int size = currentTool.thickness();
        QRect brushRect(pos.x() - size / 2, pos.y() - size / 2, size, size);
        p.drawEllipse(brushRect);
    }

    p.end();
    update();
}

void CanvasWidget::drawRectangle(const QRect &rect) {
    QPainter p(&canvasImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(currentTool.outlineColor(), currentTool.thickness()));
    p.setBrush(currentTool.fillColor());
    p.setOpacity(currentTool.opacity());
    p.drawRect(rect);
    p.end();
    update();
}

void CanvasWidget::drawEllipse(const QRect &rect) {
    QPainter p(&canvasImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(currentTool.outlineColor(), currentTool.thickness()));
    p.setBrush(currentTool.fillColor());
    p.setOpacity(currentTool.opacity());
    p.drawEllipse(rect);
    p.end();
    update();
}

void CanvasWidget::drawText(const QRect &rect) {
    QPainter p(&canvasImage);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setPen(QPen(currentTool.color()));
    p.setOpacity(currentTool.opacity());

    QFont font = currentTool.font();
    font.setBold(currentTool.bold());
    font.setItalic(currentTool.italic());
    p.setFont(font);

    p.drawText(rect, Qt::AlignCenter, currentTool.text());
    p.end();
    update();
}

void CanvasWidget::pickColor(const QPoint &pos) {
    if (canvasImage.rect().contains(pos)) {
        QColor picked = canvasImage.pixelColor(pos);
        emit colorPicked(picked);
    }
}

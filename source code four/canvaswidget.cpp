#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
      zoomFactor(1.0f),
      isDrawing(false),
      previewActive(false),
      selectionActive(false),
      tool()
{
    canvasImage = QImage(800, 600, QImage::Format_ARGB32);
    canvasImage.fill(Qt::white);
    setMinimumSize(canvasImage.size());
}

void CanvasWidget::zoomIn() {
    zoomFactor *= 1.2f;
    updateGeometry();
    update();
}

void CanvasWidget::zoomOut() {
    zoomFactor /= 1.2f;
    updateGeometry();
    update();
}

void CanvasWidget::fitToScreen() {
    if (canvasImage.isNull()) return;
    QSize areaSize = parentWidget()->size();
    float scaleX = static_cast<float>(areaSize.width()) / canvasImage.width();
    float scaleY = static_cast<float>(areaSize.height()) / canvasImage.height();
    zoomFactor = qMin(scaleX, scaleY);
    updateGeometry();
    update();
}

void CanvasWidget::resizeCanvas(int width, int height) {
    QImage newImage(width, height, QImage::Format_ARGB32);
    newImage.fill(Qt::white);
    QPainter painter(&newImage);
    painter.drawImage(0, 0, canvasImage);
    canvasImage = newImage;
    setMinimumSize(canvasImage.size());
    update();
}

Tool CanvasWidget::activeTool() const {
    return tool;
}

void CanvasWidget::setActiveTool(const Tool &newTool) {
    tool = newTool;
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    lastPoint = event->pos() / zoomFactor;
    isDrawing = true;

    if (tool.type() == ToolType::Select) {
        selectionRect.setTopLeft(lastPoint);
        selectionRect.setBottomRight(lastPoint);
        selectionActive = true;
    } else {
        previewStart = lastPoint;
        previewEnd = lastPoint;
        previewActive = true;
    }

    update();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!isDrawing) return;
    QPoint currentPoint = event->pos() / zoomFactor;

    if (tool.type() == ToolType::Select) {
        selectionRect.setBottomRight(currentPoint);
    } else {
        previewEnd = currentPoint;
    }

    update();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || !isDrawing) return;
    isDrawing = false;
    QPoint endPoint = event->pos() / zoomFactor;

    if (tool.type() == ToolType::Select) {
        selectionRect.setBottomRight(endPoint);
        selectionActive = true;
    } else {
        QPainter painter(&canvasImage);
        painter.setRenderHint(QPainter::Antialiasing);
        tool.apply(painter, previewStart, endPoint);
        previewActive = false;
    }

    update();
}
void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.scale(zoomFactor, zoomFactor);
    painter.drawImage(0, 0, canvasImage);

    if (previewActive) {
        drawPreviewShape(painter);
    }

    if (selectionActive) {
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
        painter.drawRect(selectionRect);
    }
}

void CanvasWidget::drawPreviewShape(QPainter &painter) {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(tool.outlineColor(), tool.thickness()));
    painter.setOpacity(tool.opacity());

    if (tool.fillEnabled()) {
        painter.setBrush(QBrush(tool.fillColor()));
    } else {
        painter.setBrush(Qt::NoBrush);
    }

    QRect shapeRect(previewStart, previewEnd);

    switch (tool.type()) {
        case ToolType::Line:
            painter.drawLine(previewStart, previewEnd);
            break;
        case ToolType::Rectangle:
            painter.drawRect(shapeRect);
            break;
        case ToolType::Ellipse:
            painter.drawEllipse(shapeRect);
            break;
        case ToolType::Triangle: {
            QPolygon triangle;
            triangle << previewStart
                     << QPoint(previewEnd.x(), previewStart.y())
                     << previewEnd;
            painter.drawPolygon(triangle);
            break;
        }
        case ToolType::Curve: {
            QPainterPath path;
            path.moveTo(previewStart);
            path.quadTo((previewStart + previewEnd) / 2, previewEnd);
            painter.drawPath(path);
            break;
        }
        default:
            break;
    }
}

// Imagem
void CanvasWidget::clearCanvas() {
    canvasImage.fill(Qt::white);
    update();
}

void CanvasWidget::undo() {
    // Implementar histórico se necessário
}

void CanvasWidget::redo() {
    // Implementar histórico se necessário
}

void CanvasWidget::openImage(const QString &path) {
    QImage loaded(path);
    if (!loaded.isNull()) {
        canvasImage = loaded.convertToFormat(QImage::Format_ARGB32);
        setMinimumSize(canvasImage.size());
        update();
    }
}

void CanvasWidget::saveImage(const QString &path) {
    canvasImage.save(path);
}

// Seleção (implementações vazias por enquanto)
void CanvasWidget::copySelection() {}
void CanvasWidget::cutSelection() {}
void CanvasWidget::pasteSelection() {}
void CanvasWidget::applySelection() {}
void CanvasWidget::flipSelectionHorizontal() {}
void CanvasWidget::flipSelectionVertical() {}
void CanvasWidget::rotateSelection(int) {}

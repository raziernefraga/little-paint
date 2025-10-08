#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QInputDialog>
#include <QLineEdit>
#include <QQueue>
#include <QTransform>
#include <QFileInfo>


CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
      zoomFactor(1.0f),
      isDrawing(false),
      previewActive(false),
      selectionActive(false)
{
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);

    drawingLayer = QImage(800, 600, QImage::Format_ARGB32);
    drawingLayer.fill(Qt::transparent);

    canvasImage = QImage(800, 600, QImage::Format_ARGB32);
    canvasImage.fill(Qt::transparent);

    selectionImage = QImage(1, 1, QImage::Format_ARGB32);  // ✅ inicialização segura
    selectionImage.fill(Qt::transparent);

    setMinimumSize(canvasImage.size());
    undoStack.push(canvasImage);
    
    canvasImage = QImage(this->size(), QImage::Format_ARGB32);
    canvasImage.fill(Qt::transparent);  // base transparente

    backgroundLayer = QImage(this->size(), QImage::Format_RGB32);
    backgroundLayer.fill(backgroundColor);  // fundo visível

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
    QSize areaSize = size();
    if (parentWidget())  // ✅ proteção contra nullptr
        areaSize = parentWidget()->size();

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

    undoStack.push(canvasImage);
    update();
}

void CanvasWidget::toggleGrid() {
    showGrid = !showGrid;
    update();
}

Tool CanvasWidget::activeTool() const {
    return tool;
}

Tool CanvasWidget::getTool() const {
    return tool;
}

void CanvasWidget::setActiveTool(const Tool &newTool) {
    tool = newTool;
}

void CanvasWidget::setColor(const QColor &color) {
    tool.setOutlineColor(color);
}

void CanvasWidget::setThickness(int value) {
    tool.setThickness(value);
}
void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    lastPoint = event->pos() / zoomFactor;

    if (tool.type() == ToolType::Eyedropper) {
        QPoint pos = event->pos() / zoomFactor;
        if (canvasImage.rect().contains(pos)) {
            QColor pickedColor = canvasImage.pixelColor(pos);
            if (pickedColor.isValid()) {
                if (event->button() == Qt::LeftButton) {
                    emit outlineColorPicked(pickedColor);
                } else if (event->button() == Qt::RightButton) {
                    emit fillColorPicked(pickedColor);
                }
            }
        }
        return;
    }

    // Se quiser manter o comportamento padrão para outras ferramentas:
    if (event->button() != Qt::LeftButton) return;
    isDrawing = true;
    lastPoint = event->pos() / zoomFactor;


    if (tool.type() == ToolType::Bucket) {
        QPoint seed = event->pos() / zoomFactor;
        if (!canvasImage.rect().contains(seed)) return;

        QColor targetColor = canvasImage.pixelColor(seed);
        if (!targetColor.isValid() || targetColor == tool.outlineColor()) return;  // ✅ proteção extra

        QQueue<QPoint> queue;
        queue.enqueue(seed);

        while (!queue.isEmpty()) {
            QPoint p = queue.dequeue();
            if (!canvasImage.rect().contains(p)) continue;
            if (canvasImage.pixelColor(p) != targetColor) continue;

            canvasImage.setPixelColor(p, tool.outlineColor());

            queue.enqueue(QPoint(p.x() + 1, p.y()));
            queue.enqueue(QPoint(p.x() - 1, p.y()));
            queue.enqueue(QPoint(p.x(), p.y() + 1));
            queue.enqueue(QPoint(p.x(), p.y() - 1));
        }

        undoStack.push(canvasImage);
        update();
        return;
    }

    if (tool.type() == ToolType::Text) {
        QPoint pos = event->pos() / zoomFactor;
        bool ok;
        QString text = QInputDialog::getText(this, tr("Insert Text"),
                                             tr("Text:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            QPainter painter(&canvasImage);
            painter.setPen(QPen(tool.outlineColor(), tool.thickness()));
            painter.setFont(tool.font());
            painter.drawText(pos, text);

            undoStack.push(canvasImage);
            update();
        }
        return;
    }

    isDrawing = true;
    previewStart = lastPoint;
    previewEnd = lastPoint;
    previewActive = true;

    if (tool.type() == ToolType::Select) {
        selectionRect.setTopLeft(lastPoint);
        selectionRect.setBottomRight(lastPoint);
        selectionActive = true;
    }

    update();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!isDrawing) return;

    QPoint currentPoint = event->pos() / zoomFactor;

    if (tool.type() == ToolType::Select && selectionActive) {
        selectionRect.setBottomRight(currentPoint);
    } else if (tool.type() == ToolType::Pencil || tool.type() == ToolType::Brush ||
               tool.type() == ToolType::Spray || tool.type() == ToolType::Eraser) {
        QPainter painter(&canvasImage);
        painter.setRenderHint(QPainter::Antialiasing);
        tool.apply(painter, lastPoint, currentPoint);
        lastPoint = currentPoint;
        update();  // garante que o traço apareça imediatamente
    } else {
        previewEnd = currentPoint;
        update();  // atualiza preview de formas geométricas
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || !isDrawing) return;
    isDrawing = false;
    QPoint endPoint = event->pos() / zoomFactor;

    if (tool.type() == ToolType::Select) {
        selectionRect.setBottomRight(endPoint);
        selectionActive = true;

        // ✅ proteção contra seleção nula
        if (selectionRect.isNull()) {
            selectionActive = false;
            selectionImage = QImage();
        }
} else if (tool.type() != ToolType::Pencil &&
           tool.type() != ToolType::Brush &&
           tool.type() != ToolType::Spray &&
           tool.type() != ToolType::Eraser) {
    QPainter painter(&canvasImage);
    painter.setRenderHint(QPainter::Antialiasing);
    tool.apply(painter, previewStart, endPoint);
    previewActive = false;

    undoStack.push(canvasImage);
}

    update();
}
void CanvasWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.scale(zoomFactor, zoomFactor);
    

    // Fundo visível
    if (useBackgroundImage) {
    painter.drawImage(0, 0, backgroundLayer);
}   else {
    painter.fillRect(rect(), backgroundColor);
}

    // Conteúdo desenhado
    painter.drawImage(0, 0, canvasImage);


    // Desenha as camadas
    painter.drawImage(0, 0, drawingLayer);

    // Desenha seleção se ativa
    if (selectionActive && !selectionImage.isNull() && !selectionRect.isNull()) {
        painter.drawImage(selectionRect.topLeft(), selectionImage);
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
        painter.drawRect(selectionRect);
    }

    // Desenha grade se ativada
    if (showGrid) {
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
        int step = 20 * zoomFactor;
        for (int x = 0; x < width(); x += step)
            painter.drawLine(x, 0, x, height());
        for (int y = 0; y < height(); y += step)
            painter.drawLine(0, y, width(), y);
    }

    // Desenha histórico visual
    int x = 10;
    int y = height() / zoomFactor - 85;
    for (const QImage &thumb : historyThumbnails) {
        painter.drawImage(x, y, thumb);
        x += 110;
    }

    // Desenha pré-visualização da forma
    if (previewActive && isDrawing) {
        drawPreviewShape(painter);
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
void CanvasWidget::clearCanvas() {
    canvasImage.fill(Qt::transparent);
    undoStack.push(canvasImage);
    update();
}

void CanvasWidget::undo() {
    if (undoStack.canUndo()) {
        canvasImage = undoStack.undo();
        update();
        historyThumbnails.append(canvasImage.scaled(100, 75, Qt::KeepAspectRatio));
    }
}

void CanvasWidget::redo() {
    if (undoStack.canRedo()) {
        canvasImage = undoStack.redo();
        update();
        historyThumbnails.append(canvasImage.scaled(100, 75, Qt::KeepAspectRatio));
    }
}

void CanvasWidget::openImage(const QString &path) {
    QImage loaded(path);
    if (!loaded.isNull()) {
        canvasImage = loaded.convertToFormat(QImage::Format_ARGB32);
        setMinimumSize(canvasImage.size());
        undoStack.push(canvasImage);
        update();
    }
}

void CanvasWidget::saveImage(const QString &path) {
        
    QString format = QFileInfo(path).suffix().toLower();
    QImage imageToSave;

    if (format == "png" && exportWithTransparency) {
    imageToSave = canvasImage;
    } else {
    imageToSave = composedImage();
    }

    imageToSave.save(path, format.toUpper().toUtf8().constData());
    QPainter painter(&imageToSave);

    if (useBackgroundImage) {
        painter.drawImage(0, 0, backgroundLayer);
    } else {
        painter.fillRect(imageToSave.rect(), backgroundColor);
    }

    painter.drawImage(0, 0, canvasImage);
}


bool CanvasWidget::exportImage(const QString &path, const char *format) {
    QImage imageToSave;

if (QString(format).toLower() == "png" && exportWithTransparency) {
    imageToSave = canvasImage;
} else {
    imageToSave = composedImage();
}

return imageToSave.save(path, format);

}


// Seleção
void CanvasWidget::copySelection() {
    if (!selectionActive || selectionRect.isNull()) return;
    selectionImage = canvasImage.copy(selectionRect);
}

void CanvasWidget::cutSelection() {
    if (!selectionActive || selectionRect.isNull()) return;
    selectionImage = canvasImage.copy(selectionRect);

    QPainter painter(&canvasImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(selectionRect, Qt::transparent);

    undoStack.push(canvasImage);
    update();
}

void CanvasWidget::pasteSelection() {
    if (selectionImage.isNull()) return;

    QPoint pastePos = selectionRect.topLeft();  // ou mousePos
    QPainter painter(&canvasImage);
    painter.drawImage(pastePos, selectionImage);

    undoStack.push(canvasImage);
    update();
}

void CanvasWidget::applySelection() {
    selectionActive = false;
    selectionRect = QRect();
    selectionImage = QImage(1, 1, QImage::Format_ARGB32);  // ✅ reinicialização segura
    selectionImage.fill(Qt::transparent);
    update();
    historyThumbnails.append(canvasImage.scaled(100, 75, Qt::KeepAspectRatio));
}

void CanvasWidget::flipSelectionHorizontal() {
    if (selectionImage.isNull()) return;
    selectionImage = selectionImage.mirrored(true, false);
    update();
}

void CanvasWidget::flipSelectionVertical() {
    if (selectionImage.isNull()) return;
    selectionImage = selectionImage.mirrored(false, true);
    update();
}

void CanvasWidget::rotateSelection(int angle) {
    if (selectionImage.isNull()) return;

    QTransform transform;
    transform.rotate(angle);
    selectionImage = selectionImage.transformed(transform, Qt::SmoothTransformation);
    update();
}
void CanvasWidget::setOutlineColor(const QColor &color) {
    tool.setOutlineColor(color);
}

void CanvasWidget::setFillColor(const QColor &color) {
    tool.setFillColor(color);
}
void CanvasWidget::setBackgroundColor(const QColor &color) {
    backgroundColor = color;
    useBackgroundImage = false;
    backgroundLayer.fill(color);
    update();
}

void CanvasWidget::setBackgroundImage(const QImage &image) {
    backgroundLayer = image.scaled(canvasImage.size());
    useBackgroundImage = true;
    update();
}

void CanvasWidget::clearBackgroundImage() {
    useBackgroundImage = false;
    backgroundLayer.fill(backgroundColor);
    update();
}

void CanvasWidget::setExportTransparency(bool enabled) {
    exportWithTransparency = enabled;
}
QImage CanvasWidget::composedImage() const {
    QImage result(canvasImage.size(), QImage::Format_RGB32);
    QPainter painter(&result);

    if (useBackgroundImage) {
        painter.drawImage(0, 0, backgroundLayer);
    } else {
        painter.fillRect(result.rect(), backgroundColor);
    }

    painter.drawImage(0, 0, canvasImage);
    return result;
}

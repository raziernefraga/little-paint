#include <QRandomGenerator>  // Adicione este include no topo do arquivo
#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QQueue>
#include <QTransform>
#include <QRandomGenerator>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
      m_canvas(800, 600, QImage::Format_ARGB32_Premultiplied),
      m_drawing(false),
      m_zoomFactor(1.0f),
      m_hasSelection(false),
      m_movingSelection(false)
{
    m_canvas.fill(Qt::white);
    setMinimumSize(400, 300);
}

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.scale(m_zoomFactor, m_zoomFactor);
    painter.drawImage(0, 0, m_canvas);

    if (m_drawing && (m_activeTool.type() == ToolType::Rectangle || m_activeTool.type() == ToolType::Ellipse)) {
        painter.setOpacity(m_activeTool.opacity());
        painter.setPen(QPen(m_activeTool.outlineColor(), m_activeTool.thickness()));
        painter.setBrush(m_activeTool.fillEnabled() ? QBrush(m_activeTool.fillColor()) : Qt::NoBrush);
        QRect rect(m_startPoint, m_endPoint);
        if (m_activeTool.type() == ToolType::Rectangle)
            painter.drawRect(rect);
        else
            painter.drawEllipse(rect);
    }

    if (m_hasSelection) {
        drawSelectionOutline(painter);
        if (!m_selectionImage.isNull()) {
            painter.drawImage(m_selectionRect.topLeft() + m_selectionOffset, m_selectionImage);
        }
    }
}
void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos() / m_zoomFactor;
    m_startPoint = pos;
    m_endPoint = pos;
    m_drawing = true;

    switch (m_activeTool.type()) {
        case ToolType::Bucket:
            pushUndo();
            applyFloodFill(pos, m_activeTool.fillColor());
            update();
            break;
        case ToolType::Eyedropper:
            applyEyedropper(pos);
            break;
        case ToolType::Text: {
            bool ok;
            QString text = QInputDialog::getText(this, "Insert Text", "Text:", QLineEdit::Normal, "", &ok);
            if (ok && !text.isEmpty()) {
                pushUndo();
                QPainter painter(&m_canvas);
                painter.setOpacity(m_activeTool.opacity());
                painter.setPen(QPen(m_activeTool.textColor(), m_activeTool.thickness()));
                painter.setFont(m_activeTool.font());
                painter.drawText(pos, text);
                update();
            }
            break;
        }
        case ToolType::Select:
            m_selectionRect = QRect(pos, QSize());
            m_hasSelection = true;
            m_selectionOffset = QPoint(0,0);
            break;
        default:
            break;
    }
}
void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!m_drawing) return;
    QPoint pos = event->pos() / m_zoomFactor;
    m_endPoint = pos;

    if (m_activeTool.type() == ToolType::Pencil || m_activeTool.type() == ToolType::Eraser) {
        pushUndo();
        QPainter painter(&m_canvas);
        painter.setOpacity(m_activeTool.opacity());
        QColor c = (m_activeTool.type() == ToolType::Eraser) ? Qt::white : m_activeTool.outlineColor();
        painter.setPen(QPen(c, m_activeTool.thickness(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(m_startPoint, m_endPoint);
        m_startPoint = m_endPoint;
        update();
    } else if (m_activeTool.type() == ToolType::Spray) {
        pushUndo();
        applySpray(pos);
        update();
    } else if (m_activeTool.type() == ToolType::Select) {
        m_selectionRect.setBottomRight(pos);
        update();
    } else {
        update();
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!m_drawing) return;
    QPoint pos = event->pos() / m_zoomFactor;
    m_endPoint = pos;
    m_drawing = false;

    if (m_activeTool.type() == ToolType::Rectangle || m_activeTool.type() == ToolType::Ellipse || m_activeTool.type() == ToolType::Line) {
        pushUndo();
        QPainter painter(&m_canvas);
        painter.setOpacity(m_activeTool.opacity());
        painter.setPen(QPen(m_activeTool.outlineColor(), m_activeTool.thickness()));
        painter.setBrush(m_activeTool.fillEnabled() ? QBrush(m_activeTool.fillColor()) : Qt::NoBrush);

        QRect rect(m_startPoint, m_endPoint);
        if (m_activeTool.type() == ToolType::Rectangle)
            painter.drawRect(rect);
        else if (m_activeTool.type() == ToolType::Ellipse)
            painter.drawEllipse(rect);
        else if (m_activeTool.type() == ToolType::Line)
            painter.drawLine(m_startPoint, m_endPoint);

        update();
    } else if (m_activeTool.type() == ToolType::Select) {
        m_selectionRect = QRect(m_startPoint, m_endPoint).normalized();
        if (m_selectionRect.isValid()) {
            m_selectionImage = m_canvas.copy(m_selectionRect);
        }
        update();
    }
}
void CanvasWidget::applyFloodFill(const QPoint &pt, const QColor &newColor) {
    QColor targetColor = m_canvas.pixelColor(pt);
    if (targetColor == newColor) return;

    QQueue<QPoint> queue;
    queue.enqueue(pt);

    QPainter painter(&m_canvas);
    painter.setPen(newColor);

    while (!queue.isEmpty()) {
        QPoint p = queue.dequeue();
        if (!m_canvas.rect().contains(p)) continue;
        if (m_canvas.pixelColor(p) != targetColor) continue;

        painter.drawPoint(p);

        queue.enqueue(p + QPoint(1, 0));
        queue.enqueue(p + QPoint(-1, 0));
        queue.enqueue(p + QPoint(0, 1));
        queue.enqueue(p + QPoint(0, -1));
    }
}

void CanvasWidget::applySpray(const QPoint &pos) {
    QPainter painter(&m_canvas);
    painter.setOpacity(m_activeTool.opacity());
    painter.setPen(QPen(m_activeTool.outlineColor(), 1));

    int radius = m_activeTool.thickness();
    for (int i = 0; i < 30; ++i) {
        int dx = QRandomGenerator::global()->bounded(-radius, radius + 1);
        int dy = QRandomGenerator::global()->bounded(-radius, radius + 1);
        if (dx*dx + dy*dy <= radius*radius)
            painter.drawPoint(pos + QPoint(dx, dy));
    }
}

void CanvasWidget::applyEyedropper(const QPoint &pos) {
    if (!m_canvas.rect().contains(pos)) return;
    QColor picked = m_canvas.pixelColor(pos);
    QMessageBox::information(this, "Eyedropper", "Cor capturada: " + picked.name());
}

void CanvasWidget::pushUndo() {
    m_undoStack.push(m_canvas);
    m_redoStack.clear();
}

void CanvasWidget::undo() {
    if (!m_undoStack.isEmpty()) {
        m_redoStack.push(m_canvas);
        m_canvas = m_undoStack.pop();
        update();
    }
}

void CanvasWidget::redo() {
    if (!m_redoStack.isEmpty()) {
        m_undoStack.push(m_canvas);
        m_canvas = m_redoStack.pop();
        update();
    }
}

void CanvasWidget::flipHorizontal() {
    pushUndo();
    m_canvas = m_canvas.mirrored(true, false);
    update();
}

void CanvasWidget::flipVertical() {
    pushUndo();
    m_canvas = m_canvas.mirrored(false, true);
    update();
}

void CanvasWidget::rotateCanvas(int angle) {
    pushUndo();
    QTransform transform;
    transform.rotate(angle);
    m_canvas = m_canvas.transformed(transform);
    update();
}
void CanvasWidget::zoomIn() {
    m_zoomFactor *= 1.25f;
    update();
}

void CanvasWidget::zoomOut() {
    m_zoomFactor /= 1.25f;
    update();
}

void CanvasWidget::fitToScreen() {
    if (m_canvas.isNull()) return;
    m_zoomFactor = qMin(width() / (float)m_canvas.width(),
                        height() / (float)m_canvas.height());
    update();
}

void CanvasWidget::clearCanvas() {
    pushUndo();
    m_canvas.fill(Qt::white);
    update();
}

void CanvasWidget::openImage(const QString &path) {
    QImage loaded;
    if (loaded.load(path)) {
        pushUndo();
        m_canvas = loaded.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        update();
    }
}

void CanvasWidget::saveImage(const QString &path) {
    m_canvas.save(path);
}

void CanvasWidget::resizeCanvas(int width, int height) {
    pushUndo();
    QImage newCanvas(width, height, QImage::Format_ARGB32_Premultiplied);
    newCanvas.fill(Qt::white);

    QPainter painter(&newCanvas);
    painter.drawImage(0, 0, m_canvas);
    m_canvas = newCanvas;
    update();
}
void CanvasWidget::clearSelection() {
    m_hasSelection = false;
    m_selectionImage = QImage();
    m_selectionRect = QRect();
    m_selectionOffset = QPoint();
    update();
}

void CanvasWidget::copySelection() {
    if (m_hasSelection && m_selectionRect.isValid()) {
        m_selectionImage = m_canvas.copy(m_selectionRect);
    }
}

void CanvasWidget::cutSelection() {
    if (m_hasSelection && m_selectionRect.isValid()) {
        pushUndo();
        m_selectionImage = m_canvas.copy(m_selectionRect);
        QPainter painter(&m_canvas);
        painter.fillRect(m_selectionRect, Qt::white);
        update();
    }
}

void CanvasWidget::pasteSelection() {
    if (!m_selectionImage.isNull()) {
        m_hasSelection = true;
        m_selectionRect = QRect(QPoint(10,10), m_selectionImage.size());
        m_selectionOffset = QPoint(0,0);
        update();
    }
}

void CanvasWidget::applySelection() {
    if (m_hasSelection && !m_selectionImage.isNull()) {
        pushUndo();
        QPainter painter(&m_canvas);
        painter.drawImage(m_selectionRect.topLeft() + m_selectionOffset, m_selectionImage);
        clearSelection();
        update();
    }
}

void CanvasWidget::flipSelectionHorizontal() {
    if (m_hasSelection && !m_selectionImage.isNull()) {
        m_selectionImage = m_selectionImage.mirrored(true, false);
        update();
    }
}

void CanvasWidget::flipSelectionVertical() {
    if (m_hasSelection && !m_selectionImage.isNull()) {
        m_selectionImage = m_selectionImage.mirrored(false, true);
        update();
    }
}

void CanvasWidget::rotateSelection(int angle) {
    if (m_hasSelection && !m_selectionImage.isNull()) {
        QTransform transform;
        transform.rotate(angle);
        m_selectionImage = m_selectionImage.transformed(transform);
        m_selectionRect.setSize(m_selectionImage.size());
        update();
    }
}

void CanvasWidget::drawSelectionOutline(QPainter &painter) {
    if (!m_hasSelection || !m_selectionRect.isValid()) return;

    QPen pen(Qt::DashLine);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_selectionRect.translated(m_selectionOffset));
}

void CanvasWidget::setActiveTool(const Tool &tool) {
    m_activeTool = tool;
}

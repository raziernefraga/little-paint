#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <cmath>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent),
      m_canvas(800, 600, QImage::Format_ARGB32),
      m_drawing(false),
      m_zoomFactor(1.0f),
      m_textFont("Arial", 14)
{
    m_canvas.fill(Qt::white);
    setFixedSize(m_canvas.size());
}

void CanvasWidget::setActiveTool(const Tool &tool) {
    m_activeTool = tool;
}

Tool CanvasWidget::activeTool() const {
    return m_activeTool;
}

void CanvasWidget::clearCanvas() {
    pushUndo();
    m_canvas.fill(Qt::white);
    update();
}

void CanvasWidget::openImage(const QString &path) {
    QImage img;
    if (img.load(path)) {
        pushUndo();
        m_canvas = img.scaled(size(), Qt::KeepAspectRatio);
        update();
    }
}

void CanvasWidget::saveImage(const QString &path) {
    m_canvas.save(path);
}

void CanvasWidget::copySelection() {
    if (!m_selection.isNull()) {
        m_clipboard = m_canvas.copy(m_selection);
        QApplication::clipboard()->setImage(m_clipboard);
    }
}

void CanvasWidget::cutSelection() {
    copySelection();
    QPainter painter(&m_canvas);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(m_selection, Qt::white);
    update();
}

void CanvasWidget::pasteClipboard() {
    pushUndo();
    QImage img = QApplication::clipboard()->image();
    if (!img.isNull()) {
        QPainter painter(&m_canvas);
        painter.drawImage(m_startPoint, img);
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
    m_zoomFactor *= 1.2;
    resize(m_canvas.size() * m_zoomFactor);
    update();
}

void CanvasWidget::zoomOut() {
    m_zoomFactor /= 1.2;
    resize(m_canvas.size() * m_zoomFactor);
    update();
}

void CanvasWidget::fitToScreen() {
    m_zoomFactor = 1.0;
    resize(m_canvas.size());
    update();
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

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.scale(m_zoomFactor, m_zoomFactor);
    painter.drawImage(0, 0, m_canvas);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_startPoint = event->pos() / m_zoomFactor;
        m_drawing = true;
        pushUndo();

        switch (m_activeTool.type()) {
            case ToolType::Bucket:
                m_canvas.fill(m_activeTool.color());
                update();
                break;
            case ToolType::Eyedropper:
                applyEyedropper(m_startPoint);
                break;
            case ToolType::Paste:
                pasteClipboard();
                break;
            default:
                break;
        }
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!m_drawing) return;

    QPoint pos = event->pos() / m_zoomFactor;

    switch (m_activeTool.type()) {
        case ToolType::Pencil:
        case ToolType::Eraser: {
            QPainter painter(&m_canvas);
            QColor color = (m_activeTool.type() == ToolType::Eraser) ? Qt::white : m_activeTool.color();
            QPen pen(color, m_activeTool.thickness(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);
            painter.setOpacity(m_activeTool.opacity());
            painter.drawLine(m_startPoint, pos);
            m_startPoint = pos;
            update();
            break;
        }
        case ToolType::Spray:
            applySpray(pos);
            update();
            break;
        default:
            m_endPoint = pos;
            update();
            break;
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_drawing) {
        m_endPoint = event->pos() / m_zoomFactor;
        drawShape();
        m_drawing = false;
        update();
    }
}

void CanvasWidget::drawShape() {
    QPainter painter(&m_canvas);
    painter.setPen(QPen(m_activeTool.color(), m_activeTool.thickness()));
    painter.setBrush(m_activeTool.fillEnabled() ? m_activeTool.color() : Qt::NoBrush);
    painter.setOpacity(m_activeTool.opacity());

    QRect rect(m_startPoint, m_endPoint);

    switch (m_activeTool.type()) {
        case ToolType::Rectangle:
            painter.drawRect(rect);
            break;
        case ToolType::Ellipse:
            painter.drawEllipse(rect);
            break;
        case ToolType::Line:
            painter.drawLine(m_startPoint, m_endPoint);
            break;
        case ToolType::Text:
            painter.setFont(m_textFont);
            painter.drawText(rect, Qt::AlignCenter, "Texto");
            break;
        case ToolType::Select:
            m_selection = rect;
            break;
        default:
            break;
    }
}

void CanvasWidget::applySpray(const QPoint &pos) {
    QPainter painter(&m_canvas);
    painter.setPen(QPen(m_activeTool.color()));
    painter.setOpacity(m_activeTool.opacity());

    int radius = m_activeTool.thickness();
    for (int i = 0; i < 100; ++i) {
        int dx = qrand() % (2 * radius) - radius;
        int dy = qrand() % (2 * radius) - radius;
        if (dx * dx + dy * dy <= radius * radius) {
            painter.drawPoint(pos + QPoint(dx, dy));
        }
    }
}

void CanvasWidget::applyEyedropper(const QPoint &pos) {
    QColor picked = m_canvas.pixelColor(pos);
    m_activeTool.setColor(picked);
}

void CanvasWidget::pushUndo() {
    m_undoStack.push(m_canvas);
    m_redoStack.clear();
}

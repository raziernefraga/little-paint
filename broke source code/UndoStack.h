// src/UndoStack.h
#pragma once
#include <QImage>
#include <QVector>

class UndoStack {
public:
    void clear() { stack.clear(); index = -1; }
    void push(const QImage& img) {
        while (stack.size() > index + 1) stack.removeLast();
        stack.append(img);
        index = stack.size() - 1;
    }
    bool canUndo() const { return index > 0; }
    bool canRedo() const { return index >= 0 && index < stack.size() - 1; }
    QImage undo() { if (canUndo()) --index; return stack[index]; }
    QImage redo() { if (canRedo()) ++index; return stack[index]; }
    QImage current() const { return (index >= 0) ? stack[index] : QImage(); }
private:
    QVector<QImage> stack;
    int index = -1;
};

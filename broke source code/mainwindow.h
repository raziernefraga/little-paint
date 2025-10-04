#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QActionGroup>
#include "canvaswidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Arquivo
    void newFile();
    void openFile();
    void saveFile();
    void exitApp();

    // Editar
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    // Transformações
    void flipHorizontal();
    void flipVertical();
    void rotateLeft();
    void rotateRight();
    
    // Zoom
    void zoomIn();
    void zoomOut();
    void fitToScreen();

    // Canvas
    void resizeCanvasDialog();

    // Tema
    void toggleTheme();

    // Ferramentas
    void setToolPencil();
    void setToolEraser();
    void setToolBucket();
    void setToolLine();
    void setToolRectangle();
    void setToolEllipse();
    void setToolSpray();
    void setToolText();
    void setToolEyedropper();
    void setToolSelect();

private:
    CanvasWidget *m_canvas;
    QScrollArea *m_scrollArea;

    // Ações
    QAction *newAct, *openAct, *saveAct, *exitAct;
    QAction *undoAct, *redoAct, *cutAct, *copyAct, *pasteAct;
    QAction *flipHAct, *flipVAct, *rotateLAct, *rotateRAct;
    QAction *zoomInAct, *zoomOutAct, *fitAct;
    QAction *resizeCanvasAct, *themeAct;

    QActionGroup *toolGroup;
    QAction *pencilAct, *eraserAct, *bucketAct, *lineAct, *rectAct;
    QAction *ellipseAct, *sprayAct, *textAct, *eyedropperAct, *selectAct;

    void createActions();
    void createMenus();
    void createToolbars();
    bool maybeSave();
};

#endif // MAINWINDOW_H

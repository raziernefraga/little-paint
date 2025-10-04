#include "mainwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QApplication>
#include <QStyle>  // ✅ Adicionado para corrigir o erro


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_canvas = new CanvasWidget(this);
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_canvas);
    m_scrollArea->setWidgetResizable(true);
    setCentralWidget(m_scrollArea);

    createActions();
    createMenus();
    createToolbars();

    resize(1000, 700);
    setWindowTitle("Little Paint");
}

// ======= Eventos =======

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

// ======= Arquivo =======

void MainWindow::newFile() {
    if (!maybeSave()) return;

    bool okW, okH;
    int w = QInputDialog::getInt(this, "Novo arquivo", "Largura:", 800, 100, 5000, 1, &okW);
    int h = QInputDialog::getInt(this, "Novo arquivo", "Altura:", 600, 100, 5000, 1, &okH);
    if (okW && okH) {
        m_canvas->resizeCanvas(w, h);
    }
}

void MainWindow::openFile() {
    if (!maybeSave()) return;

    QString fileName = QFileDialog::getOpenFileName(this, "Abrir imagem", "", "Imagens (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        m_canvas->openImage(fileName);
    }
}

void MainWindow::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Salvar imagem", "", "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)");
    if (!fileName.isEmpty()) {
        m_canvas->saveImage(fileName);
    }
}

void MainWindow::exitApp() {
    if (maybeSave())
        qApp->quit();
}

// ======= Editar =======

void MainWindow::undo() { m_canvas->undo(); }
void MainWindow::redo() { m_canvas->redo(); }
void MainWindow::cut() { m_canvas->cutSelection(); }
void MainWindow::copy() { m_canvas->copySelection(); }
void MainWindow::paste() { m_canvas->pasteSelection(); }

// ======= Transformações =======

void MainWindow::flipHorizontal() { m_canvas->flipHorizontal(); }
void MainWindow::flipVertical() { m_canvas->flipVertical(); }
void MainWindow::rotateLeft() { m_canvas->rotateCanvas(-90); }
void MainWindow::rotateRight() { m_canvas->rotateCanvas(90); }

// ======= Zoom =======

void MainWindow::zoomIn() { m_canvas->zoomIn(); }
void MainWindow::zoomOut() { m_canvas->zoomOut(); }
void MainWindow::fitToScreen() { m_canvas->fitToScreen(); }

// ======= Canvas =======

void MainWindow::resizeCanvasDialog() {
    bool okW, okH;
    int w = QInputDialog::getInt(this, "Redimensionar Canvas", "Largura:", m_canvas->width(), 100, 5000, 1, &okW);
    int h = QInputDialog::getInt(this, "Redimensionar Canvas", "Altura:", m_canvas->height(), 100, 5000, 1, &okH);
    if (okW && okH) {
        m_canvas->resizeCanvas(w, h);
    }
}

// ======= Tema =======

void MainWindow::toggleTheme() {
    static bool dark = false;
    dark = !dark;

    QPalette pal;
    if (dark) {
        pal.setColor(QPalette::Window, QColor(53,53,53));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(25,25,25));
        pal.setColor(QPalette::AlternateBase, QColor(53,53,53));
        pal.setColor(QPalette::ToolTipBase, Qt::white);
        pal.setColor(QPalette::ToolTipText, Qt::white);
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Button, QColor(53,53,53));
        pal.setColor(QPalette::ButtonText, Qt::white);
        pal.setColor(QPalette::BrightText, Qt::red);
    } else {
        pal = QApplication::style()->standardPalette();
    }
    QApplication::setPalette(pal);
}

// ======= Ferramentas =======

void MainWindow::setToolPencil() { m_canvas->setActiveTool(Tool(ToolType::Pencil)); }
void MainWindow::setToolEraser() { m_canvas->setActiveTool(Tool(ToolType::Eraser)); }
void MainWindow::setToolBucket() { m_canvas->setActiveTool(Tool(ToolType::Bucket)); }
void MainWindow::setToolLine() { m_canvas->setActiveTool(Tool(ToolType::Line)); }
void MainWindow::setToolRectangle() { m_canvas->setActiveTool(Tool(ToolType::Rectangle)); }
void MainWindow::setToolEllipse() { m_canvas->setActiveTool(Tool(ToolType::Ellipse)); }
void MainWindow::setToolSpray() { m_canvas->setActiveTool(Tool(ToolType::Spray)); }
void MainWindow::setToolText() { m_canvas->setActiveTool(Tool(ToolType::Text)); }
void MainWindow::setToolEyedropper() { m_canvas->setActiveTool(Tool(ToolType::Eyedropper)); }
void MainWindow::setToolSelect() { m_canvas->setActiveTool(Tool(ToolType::Select)); }

// ======= Auxiliares =======

void MainWindow::createActions() {
    // Arquivo
    newAct = new QAction("New", this);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction("Open", this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    saveAct = new QAction("Save", this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    exitAct = new QAction("Exit", this);
    connect(exitAct, &QAction::triggered, this, &MainWindow::exitApp);

    // Editar
    undoAct = new QAction("Undo", this);
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction("Redo", this);
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    cutAct = new QAction("Cut", this);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    copyAct = new QAction("Copy", this);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    pasteAct = new QAction("Paste", this);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    // Transformações
    flipHAct = new QAction("Flip H", this);
    connect(flipHAct, &QAction::triggered, this, &MainWindow::flipHorizontal);

    flipVAct = new QAction("Flip V", this);
    connect(flipVAct, &QAction::triggered, this, &MainWindow::flipVertical);

    rotateLAct = new QAction("Rotate Left", this);
    connect(rotateLAct, &QAction::triggered, this, &MainWindow::rotateLeft);

    rotateRAct = new QAction("Rotate Right", this);
    connect(rotateRAct, &QAction::triggered, this, &MainWindow::rotateRight);

    // Zoom
    zoomInAct = new QAction("Zoom In", this);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

    zoomOutAct = new QAction("Zoom Out", this);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

    fitAct = new QAction("Fit", this);
    connect(fitAct, &QAction::triggered, this, &MainWindow::fitToScreen);

    // Canvas
    resizeCanvasAct = new QAction("Resize Canvas", this);
    connect(resizeCanvasAct, &QAction::triggered, this, &MainWindow::resizeCanvasDialog);

    // Tema
    themeAct = new QAction("Toggle Theme", this);
    connect(themeAct, &QAction::triggered, this, &MainWindow::toggleTheme);

    // Ferramentas
    toolGroup = new QActionGroup(this);
    pencilAct = new QAction("Pencil", this); 
    pencilAct->setCheckable(true);
    connect(pencilAct, &QAction::triggered, this, &MainWindow::setToolPencil);
    toolGroup->addAction(pencilAct);

    eraserAct = new QAction("Eraser", this); 
    eraserAct->setCheckable(true);
    connect(eraserAct, &QAction::triggered, this, &MainWindow::setToolEraser);
    toolGroup->addAction(eraserAct);

    bucketAct = new QAction("Bucket", this); 
    bucketAct->setCheckable(true);
    connect(bucketAct, &QAction::triggered, this, &MainWindow::setToolBucket);
    toolGroup->addAction(bucketAct);

    lineAct = new QAction("Line", this); 
    lineAct->setCheckable(true);
    connect(lineAct, &QAction::triggered, this, &MainWindow::setToolLine);
    toolGroup->addAction(lineAct);

    rectAct = new QAction("Rectangle", this); 
    rectAct->setCheckable(true);
    connect(rectAct, &QAction::triggered, this, &MainWindow::setToolRectangle);
    toolGroup->addAction(rectAct);

    ellipseAct = new QAction("Ellipse", this); 
    ellipseAct->setCheckable(true);
    connect(ellipseAct, &QAction::triggered, this, &MainWindow::setToolEllipse);
    toolGroup->addAction(ellipseAct);

    sprayAct = new QAction("Spray", this); 
    sprayAct->setCheckable(true);
    connect(sprayAct, &QAction::triggered, this, &MainWindow::setToolSpray);
    toolGroup->addAction(sprayAct);

    textAct = new QAction("Text", this); 
    textAct->setCheckable(true);
    connect(textAct, &QAction::triggered, this, &MainWindow::setToolText);
    toolGroup->addAction(textAct);

    eyedropperAct = new QAction("Eyedropper", this); 
    eyedropperAct->setCheckable(true);
    connect(eyedropperAct, &QAction::triggered, this, &MainWindow::setToolEyedropper);
    toolGroup->addAction(eyedropperAct);

    selectAct = new QAction("Select", this); 
    selectAct->setCheckable(true);
    connect(selectAct, &QAction::triggered, this, &MainWindow::setToolSelect);
    toolGroup->addAction(selectAct);

    // Definir ferramenta padrão
    pencilAct->setChecked(true);
    setToolPencil();
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    QMenu *transformMenu = menuBar()->addMenu("Transform");
    transformMenu->addAction(flipHAct);
    transformMenu->addAction(flipVAct);
    transformMenu->addAction(rotateLAct);
    transformMenu->addAction(rotateRAct);

    QMenu *viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(fitAct);
    viewMenu->addSeparator();
    viewMenu->addAction(themeAct);

    QMenu *canvasMenu = menuBar()->addMenu("Canvas");
    canvasMenu->addAction(resizeCanvasAct);
}

void MainWindow::createToolbars() {
    QToolBar *fileBar = addToolBar("File");
    fileBar->addAction(newAct);
    fileBar->addAction(openAct);
    fileBar->addAction(saveAct);

    QToolBar *editBar = addToolBar("Edit");
    editBar->addAction(undoAct);
    editBar->addAction(redoAct);
    editBar->addAction(cutAct);
    editBar->addAction(copyAct);
    editBar->addAction(pasteAct);

    QToolBar *toolBar = addToolBar("Tools");
    toolBar->addAction(pencilAct);
    toolBar->addAction(eraserAct);
    toolBar->addAction(bucketAct);
    toolBar->addAction(lineAct);
    toolBar->addAction(rectAct);
    toolBar->addAction(ellipseAct);
    toolBar->addAction(sprayAct);
    toolBar->addAction(textAct);
    toolBar->addAction(eyedropperAct);
    toolBar->addAction(selectAct);

    QToolBar *transformBar = addToolBar("Transform");
    transformBar->addAction(flipHAct);
    transformBar->addAction(flipVAct);
    transformBar->addAction(rotateLAct);
    transformBar->addAction(rotateRAct);

    QToolBar *viewBar = addToolBar("View");
    viewBar->addAction(zoomInAct);
    viewBar->addAction(zoomOutAct);
    viewBar->addAction(fitAct);
    viewBar->addAction(themeAct);
}

bool MainWindow::maybeSave() {
    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "Little Paint",
        "Deseja salvar as alterações antes de continuar?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        saveFile();
        return true;
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

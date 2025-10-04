#include "mainwindow.h"
#include "canvaswidget.h"
#include "tool.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QFontComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QColorDialog>
#include <QPalette>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      currentOutlineColor(Qt::black),
      currentFillColor(Qt::white),
      currentColor(Qt::black),
      fillEnabled(false),
      thickness(2),
      opacity(1.0f),
      fontSize(12),
      boldEnabled(false),
      italicEnabled(false)
{
    canvas = new CanvasWidget(this);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(canvas);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

    currentFont = QFont("Arial", fontSize);
    currentFont.setBold(boldEnabled);
    currentFont.setItalic(italicEnabled);

    createActions();
    createMenus();
    createToolbars();
    updateTool();

    resize(1024, 768);
    setWindowTitle("LittlePaint");
}

MainWindow::~MainWindow() {}

void MainWindow::createActions() {
    // Arquivo
    newAct = new QAction("New", this);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction("Open", this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    saveAct = new QAction("Save", this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    // Edição
    undoAct = new QAction("Undo", this);
    connect(undoAct, &QAction::triggered, canvas, &CanvasWidget::undo);

    redoAct = new QAction("Redo", this);
    connect(redoAct, &QAction::triggered, canvas, &CanvasWidget::redo);

    clearAct = new QAction("Clear", this);
    connect(clearAct, &QAction::triggered, this, &MainWindow::clearCanvas);

    resizeAct = new QAction("Resize", this);
    connect(resizeAct, &QAction::triggered, this, &MainWindow::resizeCanvas);

    // Visualização
    zoomInAct = new QAction("Zoom In", this);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

    zoomOutAct = new QAction("Zoom Out", this);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

    fitAct = new QAction("Fit to Screen", this);
    connect(fitAct, &QAction::triggered, this, &MainWindow::fitToScreen);

    gridAct = new QAction("Toggle Grid", this);
    connect(gridAct, &QAction::triggered, this, &MainWindow::toggleGrid);

    themeAct = new QAction("Toggle Theme", this);
    connect(themeAct, &QAction::triggered, this, &MainWindow::toggleTheme);

    // Exportação e preferências
    exportAct = new QAction("Export", this);
    connect(exportAct, &QAction::triggered, this, &MainWindow::exportImage);

    prefsAct = new QAction("Preferences", this);
    connect(prefsAct, &QAction::triggered, this, &MainWindow::openPreferences);

    // Seleção
    copyAct = new QAction("Copy", this);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copySelection);

    cutAct = new QAction("Cut", this);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cutSelection);

    pasteAct = new QAction("Paste", this);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::pasteSelection);

    applyAct = new QAction("Apply", this);
    connect(applyAct, &QAction::triggered, this, &MainWindow::applySelection);

    flipHAct = new QAction("Flip Horizontal", this);
    connect(flipHAct, &QAction::triggered, this, &MainWindow::flipSelectionH);

    flipVAct = new QAction("Flip Vertical", this);
    connect(flipVAct, &QAction::triggered, this, &MainWindow::flipSelectionV);

    rotateLAct = new QAction("Rotate Left", this);
    connect(rotateLAct, &QAction::triggered, this, &MainWindow::rotateLeft);

    rotateRAct = new QAction("Rotate Right", this);
    connect(rotateRAct, &QAction::triggered, this, &MainWindow::rotateRight);

    // Ferramentas
    pencilAct = new QAction("Pencil", this);
    connect(pencilAct, &QAction::triggered, this, &MainWindow::setToolPencil);

    brushAct = new QAction("Brush", this);
    connect(brushAct, &QAction::triggered, this, &MainWindow::setToolBrush);

    lineAct = new QAction("Line", this);
    connect(lineAct, &QAction::triggered, this, &MainWindow::setToolLine);

    rectAct = new QAction("Rectangle", this);
    connect(rectAct, &QAction::triggered, this, &MainWindow::setToolRectangle);

    ellipseAct = new QAction("Ellipse", this);
    connect(ellipseAct, &QAction::triggered, this, &MainWindow::setToolEllipse);

    triangleAct = new QAction("Triangle", this);
    connect(triangleAct, &QAction::triggered, this, &MainWindow::setToolTriangle);

    curveAct = new QAction("Curve", this);
    connect(curveAct, &QAction::triggered, this, &MainWindow::setToolCurve);

    bucketAct = new QAction("Bucket", this);
    connect(bucketAct, &QAction::triggered, this, &MainWindow::setToolBucket);

    sprayAct = new QAction("Spray", this);
    connect(sprayAct, &QAction::triggered, this, &MainWindow::setToolSpray);

    eyedropperAct = new QAction("Eyedropper", this);
    connect(eyedropperAct, &QAction::triggered, this, &MainWindow::setToolEyedropper);

    textAct = new QAction("Text", this);
    connect(textAct, &QAction::triggered, this, &MainWindow::setToolText);

    selectAct = new QAction("Select", this);
    connect(selectAct, &QAction::triggered, this, &MainWindow::setToolSelect);

    eraserAct = new QAction("Eraser", this);
    connect(eraserAct, &QAction::triggered, this, &MainWindow::setToolEraser);
}
void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    QMenu *editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(clearAct);
    editMenu->addAction(resizeAct);

    QMenu *viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(fitAct);
    viewMenu->addAction(themeAct);
    viewMenu->addAction(gridAct);

    QMenu *selectMenu = menuBar()->addMenu("Select");
    selectMenu->addAction(copyAct);
    selectMenu->addAction(cutAct);
    selectMenu->addAction(pasteAct);
    selectMenu->addAction(applyAct);
    selectMenu->addAction(flipHAct);
    selectMenu->addAction(flipVAct);
    selectMenu->addAction(rotateLAct);
    selectMenu->addAction(rotateRAct);
}

void MainWindow::createToolbars() {
    QToolBar *toolBar = addToolBar("Tools");
    toolBar->addAction(pencilAct);
    toolBar->addAction(brushAct);
    toolBar->addAction(eraserAct);
    toolBar->addAction(lineAct);
    toolBar->addAction(rectAct);
    toolBar->addAction(ellipseAct);
    toolBar->addAction(triangleAct);
    toolBar->addAction(curveAct);
    toolBar->addAction(bucketAct);
    toolBar->addAction(sprayAct);
    toolBar->addAction(eyedropperAct);
    toolBar->addAction(textAct);
    toolBar->addAction(selectAct);
    toolBar->addAction(copyAct);
    toolBar->addAction(cutAct);
    toolBar->addAction(pasteAct);
    toolBar->addAction(applyAct);
    toolBar->addAction(flipHAct);
    toolBar->addAction(flipVAct);
    toolBar->addAction(rotateLAct);
    toolBar->addAction(rotateRAct);
    toolBar->addAction(gridAct);
    toolBar->addAction(exportAct);
    toolBar->addAction(prefsAct);

    QToolBar *styleBar = addToolBar("Style");

    QAction *outlineColorAct = new QAction("Outline Color", this);
    connect(outlineColorAct, &QAction::triggered, this, &MainWindow::changeOutlineColor);
    styleBar->addAction(outlineColorAct);

    QAction *fillColorAct = new QAction("Fill Color", this);
    connect(fillColorAct, &QAction::triggered, this, &MainWindow::changeFillColor);
    styleBar->addAction(fillColorAct);

    QAction *toggleFillAct = new QAction("Fill", this);
    toggleFillAct->setCheckable(true);
    connect(toggleFillAct, &QAction::toggled, this, &MainWindow::toggleFill);
    styleBar->addAction(toggleFillAct);

    QSpinBox *thicknessSpin = new QSpinBox(this);
    thicknessSpin->setRange(1, 50);
    thicknessSpin->setValue(thickness);
    connect(thicknessSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::changeThickness);
    styleBar->addWidget(thicknessSpin);

    QSpinBox *opacitySpin = new QSpinBox(this);
    opacitySpin->setRange(1, 100);
    opacitySpin->setValue(static_cast<int>(opacity * 100));
    connect(opacitySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::changeOpacity);
    styleBar->addWidget(opacitySpin);

    fontCombo = new QFontComboBox(this);
    connect(fontCombo, &QFontComboBox::currentFontChanged, this, &MainWindow::changeFont);
    styleBar->addWidget(fontCombo);

    fontSizeSpin = new QSpinBox(this);
    fontSizeSpin->setRange(6, 72);
    fontSizeSpin->setValue(fontSize);
    connect(fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::changeFontSize);
    styleBar->addWidget(fontSizeSpin);

    boldCheck = new QCheckBox("Bold", this);
    connect(boldCheck, &QCheckBox::toggled, this, &MainWindow::toggleBold);
    styleBar->addWidget(boldCheck);

    italicCheck = new QCheckBox("Italic", this);
    connect(italicCheck, &QCheckBox::toggled, this, &MainWindow::toggleItalic);
    styleBar->addWidget(italicCheck);
}

void MainWindow::updateTool() {
    Tool tool = canvas->activeTool();
    tool.setOutlineColor(currentOutlineColor);
    tool.setFillColor(currentFillColor);
    tool.setFillEnabled(fillEnabled);
    tool.setThickness(thickness);
    tool.setOpacity(opacity);
    QFont font = currentFont;
    font.setPointSize(fontSize);
    font.setBold(boldEnabled);
    font.setItalic(italicEnabled);
    tool.setFont(font);
    canvas->setActiveTool(tool);
}

// Visualização
void MainWindow::zoomIn() { canvas->zoomIn(); }
void MainWindow::zoomOut() { canvas->zoomOut(); }
void MainWindow::fitToScreen() { canvas->fitToScreen(); }
void MainWindow::toggleGrid() { canvas->toggleGrid(); }

// Edição
void MainWindow::undo() { canvas->undo(); }
void MainWindow::redo() { canvas->redo(); }
void MainWindow::clearCanvas() { canvas->clearCanvas(); }

// Ferramentas
void MainWindow::setToolPencil()      { Tool t; t.setType(ToolType::Pencil); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolBrush()       { Tool t; t.setType(ToolType::Brush); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolLine()        { Tool t; t.setType(ToolType::Line); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolRectangle()   { Tool t; t.setType(ToolType::Rectangle); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolEllipse()     { Tool t; t.setType(ToolType::Ellipse); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolTriangle()    { Tool t; t.setType(ToolType::Triangle); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolCurve()       { Tool t; t.setType(ToolType::Curve); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolBucket()      { Tool t; t.setType(ToolType::Bucket); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolSpray()       { Tool t; t.setType(ToolType::Spray); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolEyedropper()  { Tool t; t.setType(ToolType::Eyedropper); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolText()        { Tool t; t.setType(ToolType::Text); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolSelect()      { Tool t; t.setType(ToolType::Select); canvas->setActiveTool(t); updateTool(); }
void MainWindow::setToolEraser()      { Tool t; t.setType(ToolType::Eraser); canvas->setActiveTool(t); updateTool(); }

void MainWindow::setColorFromEyedropper(const QColor &color) {
    currentColor = color;
    canvas->setColor(color);
    updateColorPreview();  // opcional
}

void MainWindow::copySelection()  { canvas->copySelection(); }
void MainWindow::cutSelection()   { canvas->cutSelection(); }
void MainWindow::pasteSelection() { canvas->pasteSelection(); }
void MainWindow::applySelection() { canvas->applySelection(); }
void MainWindow::flipSelectionH() { canvas->flipSelectionHorizontal(); }
void MainWindow::flipSelectionV() { canvas->flipSelectionVertical(); }
void MainWindow::rotateLeft()     { canvas->rotateSelection(-90); }
void MainWindow::rotateRight()    { canvas->rotateSelection(90); }
void MainWindow::newFile() {
    bool ok;
    int width = QInputDialog::getInt(this, "Width", "Enter width:", 800, 10, 10000, 1, &ok);
    if (!ok) return;
    int height = QInputDialog::getInt(this, "Height", "Enter height:", 600, 10, 10000, 1, &ok);
    if (!ok) return;
    canvas->resizeCanvas(width, height);
    canvas->clearCanvas();
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp)");
    if (!path.isEmpty()) canvas->openImage(path);
}

void MainWindow::saveFile() {
    QString path = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
    if (!path.isEmpty()) {
        if (!path.endsWith(".png") && !path.endsWith(".jpg") && !path.endsWith(".jpeg") && !path.endsWith(".bmp")) {
            path += ".png";
        }
        canvas->saveImage(path);
    }
}

void MainWindow::resizeCanvas() {
    bool ok;
    int width = QInputDialog::getInt(this, "New Width", "Enter new width:", canvas->width(), 10, 10000, 1, &ok);
    if (!ok) return;
    int height = QInputDialog::getInt(this, "New Height", "Enter new height:", canvas->height(), 10, 10000, 1, &ok);
    if (!ok) return;
    canvas->resizeCanvas(width, height);
}

void MainWindow::exportImage() {
    QString path = QFileDialog::getSaveFileName(this, "Export Image", "", "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)");
    if (!path.isEmpty()) {
        canvas->exportImage(path);
    }
}

void MainWindow::openPreferences() {
    QDialog dialog(this);
    dialog.setWindowTitle("Preferences");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QSpinBox *thicknessBox = new QSpinBox;
    thicknessBox->setRange(1, 50);
    thicknessBox->setValue(canvas->getTool().thickness());

    QPushButton *colorButton = new QPushButton("Choose Default Color");

    layout->addWidget(new QLabel("Default Thickness:"));
    layout->addWidget(thicknessBox);
    layout->addWidget(colorButton);

    connect(colorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(canvas->getTool().outlineColor(), &dialog);
        if (color.isValid()) {
            canvas->setColor(color);
        }
    });

    connect(&dialog, &QDialog::accepted, [&]() {
        canvas->setThickness(thicknessBox->value());
    });

    dialog.exec();
}

void MainWindow::toggleTheme() {
    QPalette palette = QApplication::style()->standardPalette();
    QApplication::setPalette(palette);
}

// Estilo
void MainWindow::changeOutlineColor() {
    QColor color = QColorDialog::getColor(currentOutlineColor, this, "Choose Outline Color");
    if (color.isValid()) {
        currentOutlineColor = color;
        updateTool();
    }
}

void MainWindow::changeFillColor() {
    QColor color = QColorDialog::getColor(currentFillColor, this, "Choose Fill Color");
    if (color.isValid()) {
        currentFillColor = color;
        updateTool();
    }
}

void MainWindow::toggleFill(bool enabled) {
    fillEnabled = enabled;
    updateTool();
}

void MainWindow::changeThickness(int value) {
    thickness = value;
    updateTool();
}

void MainWindow::changeOpacity(int value) {
    opacity = value / 100.0f;
    updateTool();
}

void MainWindow::changeFont(const QFont &font) {
    currentFont.setFamily(font.family());
    updateTool();
}

void MainWindow::changeFontSize(int size) {
    fontSize = size;
    updateTool();
}

void MainWindow::toggleBold(bool checked) {
    boldEnabled = checked;
    updateTool();
}

void MainWindow::toggleItalic(bool checked) {
    italicEnabled = checked;
    updateTool();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit",
                                  "Do you want to save before exiting?",
                                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        saveFile();
        event->accept();
    } else if (reply == QMessageBox::Discard) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::updateColorPreview() {
    // Se você tiver um widget de preview de cor, atualize aqui.
    // Por enquanto, pode deixar vazio.
}

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      currentOutlineColor(Qt::black),
      currentFillColor(Qt::white),
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
    newAct = new QAction("Novo", this);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction("Abrir", this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    saveAct = new QAction("Salvar", this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    undoAct = new QAction("Desfazer", this);
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction("Refazer", this);
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    clearAct = new QAction("Limpar", this);
    connect(clearAct, &QAction::triggered, this, &MainWindow::clearCanvas);

    resizeAct = new QAction("Redimensionar", this);
    connect(resizeAct, &QAction::triggered, this, &MainWindow::resizeCanvas);

    zoomInAct = new QAction("Zoom +", this);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

    zoomOutAct = new QAction("Zoom -", this);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

    fitAct = new QAction("Ajustar à tela", this);
    connect(fitAct, &QAction::triggered, this, &MainWindow::fitToScreen);

    themeAct = new QAction("Alternar tema", this);
    connect(themeAct, &QAction::triggered, this, &MainWindow::toggleTheme);

    pencilAct = new QAction("Lápis", this);
    connect(pencilAct, &QAction::triggered, this, &MainWindow::setToolPencil);
    brushAct = new QAction("Pincel", this);
    connect(brushAct, &QAction::triggered, this, &MainWindow::setToolBrush);
    lineAct = new QAction("Linha", this);
    connect(lineAct, &QAction::triggered, this, &MainWindow::setToolLine);
    rectAct = new QAction("Retângulo", this);
    connect(rectAct, &QAction::triggered, this, &MainWindow::setToolRectangle);
    ellipseAct = new QAction("Elipse", this);
    connect(ellipseAct, &QAction::triggered, this, &MainWindow::setToolEllipse);
    triangleAct = new QAction("Triângulo", this);
    connect(triangleAct, &QAction::triggered, this, &MainWindow::setToolTriangle);
    curveAct = new QAction("Curva", this);
    connect(curveAct, &QAction::triggered, this, &MainWindow::setToolCurve);
    bucketAct = new QAction("Balde", this);
    connect(bucketAct, &QAction::triggered, this, &MainWindow::setToolBucket);
    sprayAct = new QAction("Spray", this);
    connect(sprayAct, &QAction::triggered, this, &MainWindow::setToolSpray);
    eyedropperAct = new QAction("Conta-gotas", this);
    connect(eyedropperAct, &QAction::triggered, this, &MainWindow::setToolEyedropper);
    textAct = new QAction("Texto", this);
    connect(textAct, &QAction::triggered, this, &MainWindow::setToolText);
    selectAct = new QAction("Selecionar", this);
    connect(selectAct, &QAction::triggered, this, &MainWindow::setToolSelect);

    copyAct = new QAction("Copiar", this);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copySelection);
    cutAct = new QAction("Recortar", this);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cutSelection);
    pasteAct = new QAction("Colar", this);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::pasteSelection);
    applyAct = new QAction("Aplicar", this);
    connect(applyAct, &QAction::triggered, this, &MainWindow::applySelection);
    flipHAct = new QAction("Espelhar H", this);
    connect(flipHAct, &QAction::triggered, this, &MainWindow::flipSelectionH);
    flipVAct = new QAction("Espelhar V", this);
    connect(flipVAct, &QAction::triggered, this, &MainWindow::flipSelectionV);
    rotateAct = new QAction("Rotacionar", this);
    connect(rotateAct, &QAction::triggered, this, &MainWindow::rotateSelection);
}
void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu("Arquivo");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    QMenu *editMenu = menuBar()->addMenu("Editar");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(clearAct);
    editMenu->addAction(resizeAct);

    QMenu *viewMenu = menuBar()->addMenu("Visualizar");
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(fitAct);
    viewMenu->addAction(themeAct);

    QMenu *selectMenu = menuBar()->addMenu("Seleção");
    selectMenu->addAction(copyAct);
    selectMenu->addAction(cutAct);
    selectMenu->addAction(pasteAct);
    selectMenu->addAction(applyAct);
    selectMenu->addAction(flipHAct);
    selectMenu->addAction(flipVAct);
    selectMenu->addAction(rotateAct);
}

void MainWindow::createToolbars() {
    QToolBar *toolBar = addToolBar("Ferramentas");
    toolBar->addAction(pencilAct);
    toolBar->addAction(brushAct);
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

    QToolBar *styleBar = addToolBar("Estilo");

    QAction *outlineColorAct = new QAction("Cor da linha", this);
    connect(outlineColorAct, &QAction::triggered, this, &MainWindow::changeOutlineColor);
    styleBar->addAction(outlineColorAct);

    QAction *fillColorAct = new QAction("Cor de preenchimento", this);
    connect(fillColorAct, &QAction::triggered, this, &MainWindow::changeFillColor);
    styleBar->addAction(fillColorAct);

    QAction *toggleFillAct = new QAction("Preencher", this);
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

    boldCheck = new QCheckBox("Negrito", this);
    connect(boldCheck, &QCheckBox::toggled, this, &MainWindow::toggleBold);
    styleBar->addWidget(boldCheck);

    italicCheck = new QCheckBox("Itálico", this);
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

// Ações principais
void MainWindow::newFile() {
    bool ok;
    int width = QInputDialog::getInt(this, "Largura", "Digite a largura:", 800, 10, 10000, 1, &ok);
    if (!ok) return;
    int height = QInputDialog::getInt(this, "Altura", "Digite a altura:", 600, 10, 10000, 1, &ok);
    if (!ok) return;
    canvas->resizeCanvas(width, height);
    canvas->clearCanvas();
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Abrir imagem", "", "Imagens (*.png *.jpg *.bmp)");
    if (!path.isEmpty()) canvas->openImage(path);
}

void MainWindow::saveFile() {
    QString path = QFileDialog::getSaveFileName(this, "Salvar imagem", "", "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
    if (!path.isEmpty()) {
        if (!path.endsWith(".png") && !path.endsWith(".jpg") && !path.endsWith(".jpeg") && !path.endsWith(".bmp")) {
            path += ".png";
        }
        canvas->saveImage(path);
    }
}

void MainWindow::resizeCanvas() {
    bool ok;
    int width = QInputDialog::getInt(this, "Nova largura", "Digite a nova largura:", canvas->width(), 10, 10000, 1, &ok);
    if (!ok) return;
    int height = QInputDialog::getInt(this, "Nova altura", "Digite a nova altura:", canvas->height(), 10, 10000, 1, &ok);
    if (!ok) return;
    canvas->resizeCanvas(width, height);
}

void MainWindow::toggleTheme() {
    static bool dark = false;
    dark = !dark;
    QPalette palette;
    if (dark) {
        palette.setColor(QPalette::Window, QColor(53,53,53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(25,25,25));
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53,53,53));
        palette.setColor(QPalette::ButtonText, Qt::white);
    } else {
        palette = QApplication::style()->standardPalette();
    }
    QApplication::setPalette(palette);
}
// Estilo
void MainWindow::changeOutlineColor() {
    QColor color = QColorDialog::getColor(currentOutlineColor, this, "Escolher cor da linha");
    if (color.isValid()) {
        currentOutlineColor = color;
        updateTool();
    }
}

void MainWindow::changeFillColor() {
    QColor color = QColorDialog::getColor(currentFillColor, this, "Escolher cor de preenchimento");
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

// Visualização
void MainWindow::zoomIn() {
    canvas->zoomIn();
}

void MainWindow::zoomOut() {
    canvas->zoomOut();
}

void MainWindow::fitToScreen() {
    canvas->fitToScreen();
}

// Edição
void MainWindow::undo() {
    canvas->undo();
}

void MainWindow::redo() {
    canvas->redo();
}

void MainWindow::clearCanvas() {
    canvas->clearCanvas();
}

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

// Seleção
void MainWindow::copySelection()      { canvas->copySelection(); }
void MainWindow::cutSelection()       { canvas->cutSelection(); }
void MainWindow::pasteSelection()     { canvas->pasteSelection(); }
void MainWindow::applySelection()     { canvas->applySelection(); }
void MainWindow::flipSelectionH()     { canvas->flipSelectionHorizontal(); }
void MainWindow::flipSelectionV()     { canvas->flipSelectionVertical(); }
void MainWindow::rotateSelection()    { canvas->rotateSelection(90); }

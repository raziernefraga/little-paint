#include "mainwindow.h"
#include <QFileDialog>
#include <QAction>
#include <QIcon>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      canvas(new CanvasWidget(this)),
      toolGroup(new QActionGroup(this)),
      colorDialog(new QColorDialog(this))
{
    setCentralWidget(canvas);
    createActions();
    createToolBar();
    resize(1000, 700);
}

MainWindow::~MainWindow() {}

void MainWindow::createActions() {
    QAction *newAct = new QAction(QIcon(":/icons/new.svg"), "New", this);
    QAction *openAct = new QAction(QIcon(":/icons/open.svg"), "Open", this);
    QAction *saveAct = new QAction(QIcon(":/icons/save.svg"), "Save", this);

    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    toolBar = addToolBar("Tools");
    toolBar->addAction(newAct);
    toolBar->addAction(openAct);
    toolBar->addAction(saveAct);
    toolBar->addSeparator();

    QStringList toolNames = {
        "pencil", "eraser", "bucket", "line", "rect", "ellipse", "spray",
        "text", "eyedropper", "select", "cut", "copy", "paste",
        "undo", "redo", "fliph", "flipv", "rotate", "zoomin", "zoomout", "fit"
    };

    QList<ToolType> toolTypes = {
        ToolType::Pencil, ToolType::Eraser, ToolType::Bucket, ToolType::Line,
        ToolType::Rectangle, ToolType::Ellipse, ToolType::Spray, ToolType::Text,
        ToolType::Eyedropper, ToolType::Select, ToolType::Cut, ToolType::Copy,
        ToolType::Paste, ToolType::Undo, ToolType::Redo, ToolType::FlipHorizontal,
        ToolType::FlipVertical, ToolType::Rotate, ToolType::ZoomIn,
        ToolType::ZoomOut, ToolType::FitToScreen
    };

    for (int i = 0; i < toolNames.size(); ++i) {
        QString iconPath = ":/icons/" + toolNames[i] + ".svg";
        QAction *act = new QAction(QIcon(iconPath), toolNames[i].toUpper(), this);
        act->setCheckable(true);
        act->setData(static_cast<int>(toolTypes[i]));
        toolGroup->addAction(act);
        toolBar->addAction(act);
    }

    connect(toolGroup, &QActionGroup::triggered, this, &MainWindow::setTool);

    fillToggle = new QAction(QIcon(":/icons/filltoggle.svg"), "Fill", this);
    fillToggle->setCheckable(true);
    connect(fillToggle, &QAction::toggled, this, &MainWindow::toggleFill);
    toolBar->addAction(fillToggle);

    thicknessBox = new QSpinBox(this);
    thicknessBox->setRange(1, 50);
    thicknessBox->setValue(3);
    connect(thicknessBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::setThickness);
    toolBar->addWidget(thicknessBox);

    opacityBox = new QDoubleSpinBox(this);
    opacityBox->setRange(0.1, 1.0);
    opacityBox->setSingleStep(0.1);
    opacityBox->setValue(1.0);
    connect(opacityBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::setOpacity);
    toolBar->addWidget(opacityBox);

    QAction *colorAct = new QAction(QIcon(":/icons/color.svg"), "Color", this);
    connect(colorAct, &QAction::triggered, this, &MainWindow::setColor);
    toolBar->addAction(colorAct);
}

void MainWindow::createToolBar() {
    toolBar->setMovable(false);
}

void MainWindow::newFile() {
    canvas->clearCanvas();
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Open Image");
    if (!path.isEmpty()) canvas->openImage(path);
}

void MainWindow::saveFile() {
    QString path = QFileDialog::getSaveFileName(this, "Save Image");
    if (!path.isEmpty()) canvas->saveImage(path);
}

void MainWindow::setTool() {
    QAction *act = toolGroup->checkedAction();
    if (!act) return;

    ToolType type = static_cast<ToolType>(act->data().toInt());
    Tool tool(type, canvas->activeTool().color(), thicknessBox->value(), opacityBox->value());
    tool.setFillEnabled(fillToggle->isChecked());
    canvas->setActiveTool(tool);
}

void MainWindow::setColor() {
    QColor color = colorDialog->getColor(canvas->activeTool().color(), this, "Select Color");
    if (color.isValid()) {
        Tool tool = canvas->activeTool();
        tool.setColor(color);
        canvas->setActiveTool(tool);
    }
}

void MainWindow::setThickness(int value) {
    Tool tool = canvas->activeTool();
    tool.setThickness(value);
    canvas->setActiveTool(tool);
}

void MainWindow::setOpacity(double value) {
    Tool tool = canvas->activeTool();
    tool.setOpacity(value);
    canvas->setActiveTool(tool);
}

void MainWindow::toggleFill() {
    Tool tool = canvas->activeTool();
    tool.setFillEnabled(fillToggle->isChecked());
    canvas->setActiveTool(tool);
}

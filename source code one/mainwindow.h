#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QActionGroup>
#include <QColorDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "canvaswidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();                 // Limpa o canvas
    void openFile();               // Abre uma imagem
    void saveFile();               // Salva o canvas
    void setTool();                // Aplica ferramenta selecionada
    void setColor();               // Escolhe cor
    void setThickness(int value);  // Define espessura
    void setOpacity(double value); // Define opacidade
    void toggleFill();             // Alterna preenchimento

private:
    CanvasWidget *canvas;          // Área de desenho
    QToolBar *toolBar;             // Barra de ferramentas
    QActionGroup *toolGroup;       // Grupo de ações de ferramenta
    QColorDialog *colorDialog;     // Seletor de cor
    QSpinBox *thicknessBox;        // Controle de espessura
    QDoubleSpinBox *opacityBox;    // Controle de opacidade
    QAction *fillToggle;           // Botão de preenchimento

    void createActions();          // Cria ações da interface
    void createToolBar();          // Cria barra de ferramentas
    void applyTool(ToolType type); // Aplica tipo de ferramenta
};

#endif // MAINWINDOW_H

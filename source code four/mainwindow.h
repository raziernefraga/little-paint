#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFont>
#include <QFontComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QAction>

class CanvasWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Métodos de interface
    void createActions();
    void createMenus();
    void createToolbars();
    void updateTool();

    // Ações principais
    void newFile();
    void openFile();
    void saveFile();
    void resizeCanvas();
    void toggleTheme();

    // Estilo
    void changeOutlineColor();
    void changeFillColor();
    void toggleFill(bool enabled);
    void changeThickness(int value);
    void changeOpacity(int value);
    void changeFont(const QFont &font);
    void changeFontSize(int size);
    void toggleBold(bool checked);
    void toggleItalic(bool checked);

    // Visualização
    void zoomIn();
    void zoomOut();
    void fitToScreen();

    // Edição
    void undo();
    void redo();
    void clearCanvas();

    // Ferramentas
    void setToolPencil();
    void setToolBrush();
    void setToolLine();
    void setToolRectangle();
    void setToolEllipse();
    void setToolTriangle();
    void setToolCurve();
    void setToolBucket();
    void setToolSpray();
    void setToolEyedropper();
    void setToolText();
    void setToolSelect();

    // Seleção
    void copySelection();
    void cutSelection();
    void pasteSelection();
    void applySelection();
    void flipSelectionH();
    void flipSelectionV();
    void rotateSelection();

    // Componentes
    CanvasWidget *canvas;
    QScrollArea *scrollArea;

    // Ações
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *clearAct;
    QAction *resizeAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *fitAct;
    QAction *themeAct;

    QAction *pencilAct;
    QAction *brushAct;
    QAction *lineAct;
    QAction *rectAct;
    QAction *ellipseAct;
    QAction *triangleAct;
    QAction *curveAct;
    QAction *bucketAct;
    QAction *sprayAct;
    QAction *eyedropperAct;
    QAction *textAct;
    QAction *selectAct;

    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *applyAct;
    QAction *flipHAct;
    QAction *flipVAct;
    QAction *rotateAct;

    // Estilo
    QColor currentOutlineColor;
    QColor currentFillColor;
    bool fillEnabled;
    int thickness;
    float opacity;
    QFont currentFont;
    int fontSize;
    bool boldEnabled;
    bool italicEnabled;

    QFontComboBox *fontCombo;
    QSpinBox *fontSizeSpin;
    QCheckBox *boldCheck;
    QCheckBox *italicCheck;
};

#endif // MAINWINDOW_H

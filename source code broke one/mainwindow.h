#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFont>
#include <QFontComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QAction>
#include <QColor>
#include <QPushButton>


class CanvasWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

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
    void toggleGrid();

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
    void setToolEraser();
    void setColorFromEyedropper(const QColor &color);
    void setOutlineColorFromEyedropper(const QColor &color);
    void setFillColorFromEyedropper(const QColor &color);

    // Seleção
    void copySelection();
    void cutSelection();
    void pasteSelection();
    void applySelection();
    void flipSelectionH();
    void flipSelectionV();
    void rotateLeft();
    void rotateRight();

    // Exportação e preferências
    void exportImage();
    void openPreferences();

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
    QAction *gridAct;
    QAction *exportAct;
    QAction *prefsAct;

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
    QAction *eraserAct;

    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *applyAct;
    QAction *flipHAct;
    QAction *flipVAct;
    QAction *rotateAct;
    QAction *rotateLAct;
    QAction *rotateRAct;
    
    QAction *savePngTransparent;
    QAction *savePngVisible;
    QAction *saveJpg;
    QAction *saveBmp;


    // Estilo
    QColor currentOutlineColor;
    QColor currentFillColor;
    QColor currentColor;  // usado pelo conta-gotas
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
    QPushButton *outlineColorButton;
    QPushButton *fillColorButton;


    // Utilitários
    void updateColorPreview();  // opcional, se quiser mostrar cor atual
};

#endif // MAINWINDOW_H

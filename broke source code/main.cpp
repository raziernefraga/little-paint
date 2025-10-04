#include <QApplication>
#include <QStyle>  // Corrige o erro de uso incompleto de QStyle
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Paleta padrão (tema claro)
    QPalette pal = QApplication::style()->standardPalette();
    QApplication::setPalette(pal);

    MainWindow w;
    w.show();

    return app.exec();
}

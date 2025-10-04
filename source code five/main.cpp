#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    qDebug() << "🔧 Iniciando LittlePaint...";

    QApplication app(argc, argv);
    app.setApplicationName("LittlePaint");

    qDebug() << "🖥️ Plataforma gráfica:" << QGuiApplication::platformName();

    try {
        MainWindow window;
        qDebug() << "✅ MainWindow construído com sucesso.";
        window.show();
        qDebug() << "🚀 Interface exibida. Executando loop principal...";
        return app.exec();
    } catch (const std::exception &e) {
        qCritical() << "❌ Exceção capturada:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "❌ Erro desconhecido durante execução.";
        return 2;
    }
}

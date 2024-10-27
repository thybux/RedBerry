#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Chargement du style
    QFile styleFile(":/resources/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream styleStream(&styleFile);
        a.setStyleSheet(styleStream.readAll());
        styleFile.close();
    }
    else
    {
        qWarning() << "Impossible de charger le fichier de style";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
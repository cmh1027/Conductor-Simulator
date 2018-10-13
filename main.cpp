#include "widget/mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/resources/MaestroTimes.ttf");
    MainWindow w;
    w.show();

    return a.exec();
}

#include "sqlhandler.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SqlHandler::instance();

    MainWindow w;
    w.show();

    return a.exec();
}

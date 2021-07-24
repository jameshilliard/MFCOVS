#include "mainwindow.h"
#include "ovm_200_feature.h"
#include "imagewidget.h"
#include "sqlhandler.h"

#include <QApplication>
#include <qdebug.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SqlHandler::instance();

    MainWindow w;
//    callBackNotify(&w);
    w.show();


    return a.exec();
}

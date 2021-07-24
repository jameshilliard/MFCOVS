#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString conTest = "测试";
const QString conDBMatch = "DB比对";
const QString conControl = "录入比对";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    ,_ControlForm(new ControlForm)
    ,_DBMatchForm(new DBMatchForm)
    ,_TestForm(new TestForm)
{
    ui->setupUi(this);

    resize(1280,1080);

    ui->tabWidget->clear();
    ui->tabWidget->addTab(_ControlForm,conControl);
    ui->tabWidget->addTab(_DBMatchForm,conDBMatch);
    ui->tabWidget->addTab(_TestForm,conTest);
    ui->tabWidget->setGeometry(0,0,1280,1080);
}


MainWindow::~MainWindow()
{

    delete _TestForm;
    delete _DBMatchForm;
    delete _ControlForm;
    delete ui;
}

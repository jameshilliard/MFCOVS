#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TestForm.h"
#include "MatchDB/DBMatchForm.h"
#include "MatchRealTime/ControlForm.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    TestForm    *_TestForm;
    ControlForm *_ControlForm;
    DBMatchForm *_DBMatchForm;
};

#endif // MAINWINDOW_H

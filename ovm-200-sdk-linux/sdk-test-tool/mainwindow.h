#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <map>
#include <qtimer.h>

#include "imagewidget.h"

using namespace std;

typedef vector<float> Vtrf;
typedef vector<vector<float> > vvtrf;
typedef vvtrf vtrArray;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    friend void callBackNotify(MainWindow *pMW);
    void previewHandler();
signals:
    void sigSendImage(const QImage&);
    void sigSendStage(const int);

private slots:
    void slotTimerOut();

    void on_pushButton_startEnroll_clicked();
    void on_pushButton_abortEnroll_clicked();

    void on_pushButton_startMatch_clicked();
    void on_pushButton_abortMatch_clicked();
    void on_pushButton_write2Flash_clicked();

    void on_pushButton_readFromFlash_clicked();
private:
    void readFlashData();
    void toolReset();
    static void sGetPreview(OVS::STATUS,
                            OVS::Image&,
                            std::string&,
                            OVS::Information&);

    void ovsInfoHandler();
    void ovsFeatureHandler();
    void updateIdList();

    void str2VtrfFeature(const string &str, Vtrf &vtrf);
    void str2VvtrTemplate(const string &srcStr,
                                        vvtrf &featureVtr);
private:
    enum E_RUN_TYPE{
        e_null,
        e_enroll,
        e_match
    };
private:
    E_RUN_TYPE _softRunType;

    static OVS::STATUS _ovsStatus;
    static OVS::Image _ovsImage;
    static OVS::Information _ovsInfo;
    static std::string _ovsFeature;

    OVS::Information _ovsPreInfo;

    QTimer *_timer;

    Ui::MainWindow *ui;
    ImageWidget *_imageWidget;

    map<string,string> _stdMap;
};

void callBackNotify(MainWindow *pMW);

#endif // MAINWINDOW_H

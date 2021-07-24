#ifndef THREADHANDLER_H
#define THREADHANDLER_H

#include <QObject>
#include <QThread>
#include <functional>
#include <algorithm>

#include "Global.h"
#include "algorithm/include/aux_utils.h"

void LogOut(QString str, int Level);

extern bool bFeatureLow;
extern bool bFeatureMid;
extern bool bFeatureMid1;
extern bool bFeatureMid2;
extern bool bFeatureHigh;

class ThreadDis : public QObject
{
    Q_OBJECT
public:
    ThreadDis();
    static ThreadDis *instance();
    ~ThreadDis();

    bool _thdDisRunFlag;
private:

    void ThreadStart();

public slots:
    //内部信号槽
    void slot_onThreadDisRun();
};

class ThreadAlgo : public QObject
{
    Q_OBJECT
public:
    ThreadAlgo();
    static ThreadAlgo *instance();
    ~ThreadAlgo();

    bool _thdAlgoRunFlag;
    AuxDev::StatusInfo _StatusInfo;

    bool ClearEnrollData();

signals:
    void sigFeatureResult(int iWhichFeature);//1:近模板  2:中近模板  3:中模板  4:中远模板  5:远模板  6:录入停止
    void sigExecuteResult(const AuxDev::StatusInfo,OVSInformation,bool bFinalSave);

private:
    std::string featureOutL,featureOutM,featureOutM1,featureOutM2,featureOutH;

    int iOldDisState;//1:40~60    2:60~68    3:68~76    4:76~85    5:85~115
    int iNewDisState;//1:40~60    2:60~68    3:68~76    4:76~85    5:85~115

    bool bSQLSaveImg;
    bool _enrollStageUpdateFlag;

    void ThreadStart();
    bool SaveDiffDisImage(bool &bDisFea,int dis,std::string &featureOut,const std::string &featureIn);
    bool checkUpdate1EnrollProgress(const std::string &featureIn,std::string &featureOut);
    bool checkUpdateNEnrollProgress(const std::string &featureIn,std::string &featureOut);
    bool checkUpdateEnrollProgress(int dis,const std::string &featureIn,std::string &featureOut,
                                          std::string &featureOut1,std::string &featureOut2,
                                          std::string &featureOut3,std::string &featureOut4,std::string &featureOut5);

public slots:
    //内部信号槽
    void slot_onThreadAlgoRun();
};

class ThreadImage : public QObject
{
    Q_OBJECT
public:
    ThreadImage();
    static ThreadImage *instance();
    ~ThreadImage();

    bool _thdImageRunFlag;

private:

    void ThreadStart();

public slots:
    //内部信号槽
    void slot_onThreadImageRun();
};

class ThreadHandler : public QObject
{
    Q_OBJECT
public:
    ThreadHandler();
    static ThreadHandler *instance();
    ~ThreadHandler();

    void ThreadStart();
    void ThreadStop();
    void ThreadStopTest();

private:
    QThread *_QthrDis;
    QThread *_QthrAlgo;
    QThread *_QthrImage;
    ThreadDis   *pThreadDis;
    ThreadAlgo  *pThreadAlgo;
    ThreadImage *pThreadImage;

};

#endif // THREADHANDLER_H

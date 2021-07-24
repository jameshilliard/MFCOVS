#include "Log.h"
#include "DevHandler.h"
#include "MatchRealTime/ThreadHandler.h"
#include "algorithmhandler.h"
#include "opencv2/opencv.hpp"
#include "algorithm/include/utils.hpp"
#include "algorithm/include/palm_vein.hpp"

#include <chrono>
#include <QDebug>
#include <QDateTime>

//int i = 0;

bool bFeatureLow   = false;
bool bFeatureMid   = false;
bool bFeatureMid1  = false;
bool bFeatureMid2  = false;
bool bFeatureHigh  = false;

//bool _thdImageRunFlag = true;
//bool _thdAlgoRunFlag  = true;
//bool _thdDisRunFlag   = true;

//AuxDev::StatusInfo ThreadAlgo::_StatusInfo;
//bool ThreadDis::_thdDisRunFlag    = true;
//bool ThreadAlgo::_thdAlgoRunFlag  = true;
//bool ThreadImage::_thdImageRunFlag = true;

//timer interval
const int conDisIntervaleMs      = 5;
const int conImageIntervalMs     = 5;
const int conAlgorithmIntervalMs = 5;

//bool ThreadAlgo::bSQLSaveImg            = false;
//bool ThreadAlgo::_enrollStageUpdateFlag = false;
//std::string ThreadAlgo::featureOutL,ThreadAlgo::featureOutM
//,ThreadAlgo::featureOutM1,ThreadAlgo::featureOutM2,ThreadAlgo::featureOutH;

void LogOut(QString str, int Level)
{
    switch (Level)
    {
    case LOG_DEBUG:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->debug(str);
        break;
    case LOG_INFO:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->info(str);
        break;
    case LOG_WARING:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->warn(str);
        break;
    case LOG_ERROR:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->error(str);
        break;
    default:
        break;
    }
}

ThreadHandler::ThreadHandler()
{
    _QthrDis   = new QThread();
    _QthrAlgo  = new QThread();
    _QthrImage = new QThread();

//    ThreadDis::instance()->_thdDisRunFlag     = true;
//    ThreadAlgo::instance()->_thdAlgoRunFlag   = true;
//    ThreadImage::instance()->_thdImageRunFlag = true;

    ThreadDis::instance()->moveToThread(_QthrDis);
    ThreadAlgo::instance()->moveToThread(_QthrAlgo);
    ThreadImage::instance()->moveToThread(_QthrImage);

    QObject::connect(_QthrDis,   SIGNAL(started()), ThreadDis::instance(),   SLOT(slot_onThreadDisRun()));
    QObject::connect(_QthrAlgo,  SIGNAL(started()), ThreadAlgo::instance(),  SLOT(slot_onThreadAlgoRun()));
    QObject::connect(_QthrImage, SIGNAL(started()), ThreadImage::instance(), SLOT(slot_onThreadImageRun()));


    //    ThreadStart();
}

ThreadHandler *ThreadHandler::instance()
{
    static ThreadHandler ins;
    return &ins;
}

ThreadHandler::~ThreadHandler()
{
    ThreadDis::instance()->_thdDisRunFlag     = false;
    ThreadAlgo::instance()->_thdAlgoRunFlag   = false;
    ThreadImage::instance()->_thdImageRunFlag = false;

    if(_QthrDis->isRunning())
    {
        _QthrDis->quit();
        _QthrDis->wait();
        delete _QthrDis;
    }

    if(_QthrAlgo->isRunning())
    {
        _QthrAlgo->quit();
        _QthrAlgo->wait();
        delete _QthrAlgo;
    }

    if(_QthrImage->isRunning())
    {
        _QthrImage->quit();
        _QthrImage->wait();
        delete _QthrImage;
    }
}

void ThreadHandler::ThreadStart()
{
    ThreadDis::instance()->_thdDisRunFlag     = true;
    ThreadAlgo::instance()->_thdAlgoRunFlag   = true;
    ThreadImage::instance()->_thdImageRunFlag = true;

    if(!_QthrDis->isRunning())
        _QthrDis->start();

    if(!_QthrAlgo->isRunning())
        _QthrAlgo->start();

    if(!_QthrImage->isRunning())
        _QthrImage->start();

    QString str = "ThreadStart  ,_QthrDis isruning:"   + QString::number(_QthrDis->isRunning()) +
            ",_QthrAlgo isruning:"  + QString::number(_QthrAlgo->isRunning()) +
            ",_QthrImage isruning:" + QString::number(_QthrImage->isRunning());
    LogOut(str,LOG_DEBUG);
}

void ThreadHandler::ThreadStop()
{
    ThreadDis::instance()->_thdDisRunFlag     = false;
    ThreadAlgo::instance()->_thdAlgoRunFlag   = false;
    ThreadImage::instance()->_thdImageRunFlag = false;

    if(_QthrDis->isRunning())
    {
        _QthrDis->quit();
        _QthrDis->wait();
    }

    if(_QthrAlgo->isRunning())
    {
        _QthrAlgo->quit();
        _QthrAlgo->wait();
    }

    if(_QthrImage->isRunning())
    {
        _QthrImage->quit();
        _QthrImage->wait();
    }
}

void ThreadHandler::ThreadStopTest()
{
//    ThreadDis::instance()->_thdDisRunFlag     = false;
    ThreadAlgo::instance()->_thdAlgoRunFlag   = false;
//    ThreadImage::instance()->_thdImageRunFlag = false;

//    if(_QthrDis->isRunning())
//    {
//        _QthrDis->quit();
//        _QthrDis->wait();
//    }

    if(_QthrAlgo->isRunning())
    {
        _QthrAlgo->quit();
        _QthrAlgo->wait();
    }

//    if(_QthrImage->isRunning())
//    {
//        _QthrImage->quit();
//        _QthrImage->wait();
//    }
}

ThreadDis::ThreadDis()
{

}

ThreadDis *ThreadDis::instance()
{
    static ThreadDis ins;
    return &ins;
}

ThreadDis::~ThreadDis()
{

}

void ThreadDis::slot_onThreadDisRun()
{
    LogOut("ThreadDis slot_onThreadDisRun",LOG_DEBUG);

    while(_thdDisRunFlag)
    {
        qDebug()<<"slot_onThreadDisRun 00";

        int dis,m_ActualDistanceValue;
        DevHandler::instance()->getDistance(g_sensorDis);
        qDebug()<<"slot_onThreadDisRun 11";

        dis = g_sensorDis;
        qDebug()<<"slot_onThreadDisRun 22";
        if((dis > g_iDisSensorLow) && (dis < g_iDisSensorHigh))
        {
            if((dis > g_iDisSensorLow) && (dis < g_iDisSensorLowH))
                m_ActualDistanceValue = 4;
            else if((dis >= g_iDisSensorLowH) && (dis <= g_iDisSensorHighL))
                m_ActualDistanceValue = 7;
            else
                m_ActualDistanceValue = 10;
            qDebug()<<"slot_onThreadDisRun 33 m_ActualDistanceValue = "<<m_ActualDistanceValue;

            switch (m_ActualDistanceValue)
                {
                case 3:
                case 4:
                    DevHandler::instance()->setExposure(14);
                    break;
                case 5:
                case 6:
                case 7:
                    DevHandler::instance()->setExposure(24);
                    break;
                case 8:
                case 9:
                case 10:
                    DevHandler::instance()->setExposure(45);
                    break;
                default:
                    break;
                }
        }
        qDebug()<<"slot_onThreadDisRun 44";

        QThread::msleep(conDisIntervaleMs);
    }

    LogOut("ThreadDis slot_onThreadDisRun stop",LOG_DEBUG);

}

ThreadAlgo::ThreadAlgo()
{

}

ThreadAlgo *ThreadAlgo::instance()
{
    static ThreadAlgo ins;
    return &ins;
}

ThreadAlgo::~ThreadAlgo()
{

}

//一个模板
bool ThreadAlgo::checkUpdate1EnrollProgress(const std::string &featureIn,std::string &featureOut)
{
    qDebug()<<"g_iEnrollFeatureCount = "<<g_iEnrollFeatureCount<<",conIEnrollFeatureCount = "<<conIEnrollFeatureCount;
    if(g_iEnrollFeatureCount < conIEnrollFeatureCount - 1)
    {
        g_iEnrollFeatureCount++;

        int tmpProgress = g_iEnrollFeatureCount * (100 / conIEnrollFeatureCount);

        if(tmpProgress > g_ovsInfo.enroll_progress)
        {
            if(tmpProgress == 100)
            {
                tmpProgress = 95;
            }

            g_ovsInfo.enroll_progress = tmpProgress;
        }
        return false;
    }
    else
    {
        featureOut.append(featureIn);
        g_ovsInfo.enroll_progress = 0;
        g_iEnrollFeatureCount = 0;
        return true;
    }
}

//五个模板
bool ThreadAlgo::checkUpdateNEnrollProgress(const std::string &featureIn, std::string &featureOut)
{
    qDebug()<<"g_iEnrollFeatureCount = "<<g_iEnrollFeatureCount;

    if(g_iEnrollFeatureCount < conIEnrollFeatureCount - 1)
    {
        g_iEnrollFeatureCount++;

        featureOut.append(featureIn);

        int tmpProgress = g_iEnrollFeatureCount * (100 / conIEnrollFeatureCount);

        if(tmpProgress > g_ovsInfo.enroll_progress)
        {
            if(tmpProgress == 100)
            {
                tmpProgress = 95;
            }

            g_ovsInfo.enroll_progress = tmpProgress;
        }
        return false;
    }
    else
    {
        featureOut.append(featureIn);
        g_ovsInfo.enroll_progress = 0;
        g_iEnrollFeatureCount = 0;
        return true;
    }
}

bool ThreadAlgo::SaveDiffDisImage(bool &bDisFea, int dis, string &featureOut, const string &featureIn)
{
    g_iEnrollFeatureCount++;

    if((g_iEnrollFeatureCount < (conIEnrollFeatureCount - 1)))
        return false;

    if(featureOut.size() <= 0)
    {
        qDebug()<<"featureOut.append(featureIn) dis = "<<dis <<" 进行到第 3 步 ";
        featureOut.append(featureIn);

        g_ovsInfo.iDistance = dis;
        emit sigExecuteResult(_StatusInfo,g_ovsInfo,bSQLSaveImg);

        if((dis >= g_iDisSensorLowH) && (dis < g_iDisSensorLowH1))//60~68
            emit sigFeatureResult(2);

        if((dis >= g_iDisSensorLowH1) && (dis < g_iDisSensorLowH2))//68~76
            emit sigFeatureResult(3);

        if((dis >= g_iDisSensorLowH2) && (dis < g_iDisSensorHighL))//76~85
            emit sigFeatureResult(4);

        if((dis >= g_iDisSensorLow) && (dis < g_iDisSensorLowH))//40~60
            emit sigFeatureResult(1);

        if((dis >= g_iDisSensorHighL) && (dis < g_iDisSensorHigh))//85~115
            emit sigFeatureResult(5);

        bDisFea  = true;
    }
    else
    {
        bSQLSaveImg = true;
        g_iEnrollFeatureCount = 0;
    }
//    qDebug()<<"1 featureOut.size() = "<<featureOut.size();
}

//近中远各取N个模板
bool ThreadAlgo::checkUpdateEnrollProgress(int dis, const string &featureIn, string &featureOut, std::string &featureOut1,
                                           std::string &featureOut2, std::string &featureOut3, std::string &featureOut4,
                                            std::string &featureOut5)
{
    qDebug()<<"bFeatureLow = "<<bFeatureLow<<" | bFeatureMid = "<<bFeatureMid
           <<" | bFeatureMid1 = "<<bFeatureMid1<<" | bFeatureMid2 = "<<bFeatureMid2
          <<" | bFeatureHigh = "<<bFeatureHigh<<" | dis = "<<dis;

#if 1//模板远、近各一个，   中的模板不同距离各取一个
    if((false == bFeatureMid) || (false == bFeatureMid1) || (false == bFeatureMid2) ||
            (false == bFeatureLow)  || (false == bFeatureHigh))
    {
        if((dis >= g_iDisSensorLowH) && (dis < g_iDisSensorLowH1))//60~68
            iNewDisState = 2;
        if((dis >= g_iDisSensorLowH1) && (dis < g_iDisSensorLowH2))//68~76
            iNewDisState = 3;
        if((dis >= g_iDisSensorLowH2) && (dis < g_iDisSensorHighL))//76~85
            iNewDisState = 4;
        if((dis >= g_iDisSensorLow) && (dis < g_iDisSensorLowH))//40~60
            iNewDisState = 1;
//        if((dis >= g_iDisSensorHighL) && (dis < g_iDisSensorHigh))//85~115
//        if((dis >= 105) && (dis < 115))//85~115
        if((dis >= g_iDisSensorHighL) && (dis < 95))//85~115
            iNewDisState = 5;

        qDebug()<<"iOldDisState = "<<iOldDisState<<",iNewDisState = "<<iNewDisState<<",g_iEnrollFeatureCount = "<<g_iEnrollFeatureCount;
        if(iOldDisState != iNewDisState)
            g_iEnrollFeatureCount = 0;

        if((dis >= g_iDisSensorLowH) && (dis < g_iDisSensorLowH1))//60~68
        {
            iOldDisState = 2;
            SaveDiffDisImage(bFeatureMid,dis,featureOutM,featureIn);
        }

        if((dis >= g_iDisSensorLowH1) && (dis < g_iDisSensorLowH2))//68~76
        {
            iOldDisState = 3;
            SaveDiffDisImage(bFeatureMid1,dis,featureOutM1,featureIn);
        }

        if((dis >= g_iDisSensorLowH2) && (dis < g_iDisSensorHighL))//76~85
        {
            iOldDisState = 4;
            SaveDiffDisImage(bFeatureMid2,dis,featureOutM2,featureIn);
        }

        if((dis >= g_iDisSensorLow) && (dis < g_iDisSensorLowH))//40~60
        {
            iOldDisState = 1;
            SaveDiffDisImage(bFeatureLow,dis,featureOutL,featureIn);
        }

//        if((dis >= g_iDisSensorHighL) && (dis < g_iDisSensorHigh))//85~115
//        if((dis >= 105) && (dis < 115))//85~115
        if((dis >= g_iDisSensorHighL) && (dis < 95))//85~115
        {
            iOldDisState = 5;
            SaveDiffDisImage(bFeatureHigh,dis,featureOutH,featureIn);
        }

#endif

#if 0 //近1中3远1各取131个稳定模板模板
        if((false == FeatureLow)  || (false == FeatureMid) || (false == FeatureHigh))
        {
            qDebug()<<"featureOutL = "<<featureOutL.size()<<"featureOutM = "
                   <<featureOutM.size()<<",featureOutH = "<<featureOutH.size()<<",g_iEnrollFeatureCount = "<<g_iEnrollFeatureCount;

            if((dis > g_iDisSensorLow) && (dis < g_iDisSensorLowH))
            {
                g_iEnrollFeatureCount++;
                //            qDebug()<<"0featureIn.size() = "<<featureIn.size()<<"featureOutL.size() = "<<featureOutL.size();

                if((g_iEnrollFeatureCount < (conIEnrollFeatureCount - 1)))
                    return false;

                if(featureOutL.size() < 2304)
                    featureOutL.append(featureIn);
                else
                {
                    FeatureLow = true;
                    g_iEnrollFeatureCount = 0;
                }
            }

            if((dis > g_iDisSensorLowH) && (dis < g_iDisSensorHighL))
            {
                g_iEnrollFeatureCount++;
                qDebug()<<"1 featureIn.size() = "<<featureIn.size();

                if((g_iEnrollFeatureCount < (conIEnrollFeatureCount - 1)))
                    return false;

                if(featureOutM.size() < 6912)
                    featureOutM.append(featureIn);
                else
                {
                    FeatureMid = true;
                    g_iEnrollFeatureCount = 0;
                }
                qDebug()<<"1 featureOutM.size() = "<<featureOutM.size();
            }

            if((dis > g_iDisSensorHighL) && (dis < g_iDisSensorHigh))
            {
                g_iEnrollFeatureCount++;

                if((g_iEnrollFeatureCount < (conIEnrollFeatureCount - 1)))
                    return false;

                if(featureOutH.size() < 2304)
                    featureOutH.append(featureIn);
                else
                {
                    FeatureHigh = true;
                    g_iEnrollFeatureCount = 0;
                }
            }

#endif

#if 0 //近中远各取一个稳定模板
            if((false == FeatureLow)  || (false == FeatureMid) || (false == FeatureHigh))
            {
                g_iEnrollFeatureCount++;

                if((dis > g_iDisSensorLow) && (dis < g_iDisSensorLowH) && (false == FeatureLow) &&
                        (g_iEnrollFeatureCount > conIEnrollFeatureCount - 1))
                {
                    featureOut.append(featureIn);
                    FeatureLow  = true;
                    bSQLSaveImg = true;
                    g_iEnrollFeatureCount = 0;
                }
                else if((dis > g_iDisSensorLowH) && (dis < g_iDisSensorHighL) && (false == FeatureMid) &&
                        (g_iEnrollFeatureCount > conIEnrollFeatureCount - 1))
                {
                    featureOut.append(featureIn);
                    FeatureMid  = true;
                    bSQLSaveImg = true;
                    g_iEnrollFeatureCount = 0;
                }
                else if((dis > g_iDisSensorHighL) && (dis < g_iDisSensorHigh) && (false == FeatureHigh) &&
                        (g_iEnrollFeatureCount > conIEnrollFeatureCount - 1))
                {
                    featureOut.append(featureIn);
                    FeatureHigh = true;
                    bSQLSaveImg = true;
                    g_iEnrollFeatureCount = 0;
                }
#endif

#if 0
                int tmpProgress = g_iEnrollFeatureCount * (100 / conIEnrollFeatureCount);

                if(tmpProgress > g_ovsInfo.enroll_progress)
                {
                    if(tmpProgress == 100)
                    {
                        tmpProgress = 95;
                    }

                    g_ovsInfo.enroll_progress = tmpProgress;
                }
#endif

                if((true == bFeatureLow) && (true == bFeatureMid) && (true == bFeatureHigh) &&
                        (true == bFeatureMid1) && (true == bFeatureMid2))
                {
    //                featureOut  = featureOutM + featureOutM1 + featureOutM2 + featureOutH + featureOutL;
                    featureOut  = featureOutL;
                    featureOut1 = featureOutM;
                    featureOut2 = featureOutM1;
                    featureOut3 = featureOutM2;
                    featureOut4 = featureOutH;

                    ClearEnrollData();
                    return true;
                }

                return false;
            }

//            if((true == bFeatureLow) && (true == bFeatureMid) && (true == bFeatureHigh) &&
//                    (true == bFeatureMid1) && (true == bFeatureMid2))
//            {
////                featureOut  = featureOutM + featureOutM1 + featureOutM2 + featureOutH + featureOutL;
//                featureOut  = featureOutL;
//                featureOut1 = featureOutM;
//                featureOut2 = featureOutM1;
//                featureOut3 = featureOutM2;
//                featureOut4 = featureOutH;

//                ClearEnrollData();
//                return true;
//            }
        }

        bool ThreadAlgo::ClearEnrollData()
        {
            featureOutL.clear();
            featureOutM.clear();
            featureOutH.clear();
            featureOutM1.clear();
            featureOutM2.clear();

            bFeatureLow  = false;
            bFeatureMid  = false;
            bFeatureMid1 = false;
            bFeatureMid2 = false;
            bFeatureHigh = false;
            g_ovsInfo.enroll_progress = 0;
            g_iEnrollFeatureCount = 0;
        }

        void ThreadAlgo::slot_onThreadAlgoRun()
        {

            std::string featureRes,featureRes1,featureRes2,featureRes3,featureRes4,featureRes5;
            LogOut("ThreadDis slot_onThreadAlgoRun",LOG_DEBUG);

            while(_thdAlgoRunFlag)
            {
#if 1
                static int preDisEnableTimes = 0;

                int dis     = g_sensorDis;
                bSQLSaveImg = false;

                if((dis > g_iDisSensorLow) && (dis < g_iDisSensorHigh))
                {
                    //
                    if(_enrollStageUpdateFlag)
                    {
                        if(preDisEnableTimes < conPreDisEnableTimes)
                            continue;
                        else
                        {
                            _enrollStageUpdateFlag = false;
                            preDisEnableTimes = 0;
                        }
                    }

                    //
                    cv::Mat matTmp;
                    std::string feature;

                    {
                        //                std::lock_guard<std::mutex> lkd(g_mtxMatRaw);
                        if(g_matRaw.empty()) {
                            continue;
                        }
                        g_matRaw.copyTo(matTmp);
                        qDebug()<<"进行到第 1 步";
                    }

                    _StatusInfo.image_roi = cv::Mat::zeros(cv::Size(450,450),CV_8UC1);
                    _StatusInfo.image_wnd = cv::Mat::zeros(cv::Size(450,450),CV_8UC1);

                    QElapsedTimer msTimerGetFeature;
                    msTimerGetFeature.start();

                    int ret = AlgorithmHandler::instance()->
                            execute(matTmp,g_palmImageInfo,feature,_StatusInfo);
                    float fTimerGetFeature = (double)msTimerGetFeature.nsecsElapsed()/(double)1000000;
                    qDebug()<<"One feature.size = "<<feature.size();

                    if(true == ret)
                    {
                        _StatusInfo.a6_1fetturesucc.success      = true;
                        _StatusInfo.a6_1fetturesucc.time_consume = fTimerGetFeature;

                        _mutex.lock();
                        matTmp.copyTo(_StatusInfo.image_raw);
                        qDebug()<<"进行到第 2 步";
                        _mutex.unlock();
                    }
                    else
                        _StatusInfo.a6_1fetturesucc.success      = false;
                    //            qDebug()<<"AlgorithmHandler execute fTimerGetFeature = "<<fTimerGetFeature<<",ret = "<<ret;

                    if(true == ret)
                    {
                        //                logde<<"exec true +++++++++++++++++";
                        qDebug()<<"exec true +++++++++++++++++  TimerGetFeature = "<<fTimerGetFeature;
                        _mutex.lock();
                        ListTimeoutMat.append(matTmp);
                        _mutex.unlock();

                        g_ovsInfo.iMatchImgNumSucc ++;
                        //
                        g_ovsInfo.palm_cx = g_palmImageInfo.center_x;
                        g_ovsInfo.palm_cy = g_palmImageInfo.center_y;

//                        g_ovsInfo._StatusInfo = _StatusInfo;
                        //
                        if(g_sdkStatus & E_SDK_STATUS::e_enroll)
                        {   
                            //
#if 0
                            updateStageProgress();
#endif
                            //
                            //                    if(checkUpdate1EnrollProgress(feature,featureRes))
                            featureRes.clear();
                            featureRes1.clear();
                            featureRes2.clear();
                            featureRes3.clear();
                            featureRes4.clear();

                            //判断手掌中心位置是否在小圆内
                            if(g_ovsInfo.palm_cx <= 40 || g_ovsInfo.palm_cx >= 60 ||
                               g_ovsInfo.palm_cy <= 50 || g_ovsInfo.palm_cy >= 70)
                            {
                                qDebug()<<"掌心未在小圆内 palm_cx = "<<g_ovsInfo.palm_cx<<"，palm_cy = "<<g_ovsInfo.palm_cy;
                                continue;
                            }


                            if(checkUpdateEnrollProgress(dis,feature,featureRes,featureRes1,featureRes2,featureRes3,
                                                         featureRes4,featureRes5))
                            {
                                OVS::PalmVeinTemplate builder(0);

                                Vtrf vtrsFeatureL,vtrsFeatureM,vtrsFeatureM1,vtrsFeatureM2,vtrsFeatureH;

                                qDebug()<<"featureRes.size() = "<<featureRes.size()<<",featureRes1.size() = "<<featureRes1.size();
                                Global::instance()->str2VtrTemplate(featureRes,vtrsFeatureL);
                                Global::instance()->str2VtrTemplate(featureRes1,vtrsFeatureM);
                                Global::instance()->str2VtrTemplate(featureRes2,vtrsFeatureM1);
                                Global::instance()->str2VtrTemplate(featureRes3,vtrsFeatureM2);
                                Global::instance()->str2VtrTemplate(featureRes4,vtrsFeatureH);

                                bool ok = false;
                                int iPtsNum = 0;

                                while (!ok)
                                {
                                    builder.feed(vtrsFeatureL.data(), vtrsFeatureL.size());
                                    builder.feed(vtrsFeatureM.data(), vtrsFeatureM.size());
                                    builder.feed(vtrsFeatureM1.data(),vtrsFeatureM1.size());
                                    builder.feed(vtrsFeatureM2.data(),vtrsFeatureM2.size());
                                    builder.feed(vtrsFeatureH.data(), vtrsFeatureH.size());

                                    iPtsNum = builder.check(ok);

                                    qDebug()<<"ok = "<<ok;
                                    qDebug()<<"iPtsNum = "<<iPtsNum;
                                }


                                vector<float> FinalTemp;
                                builder.buildTemplate(FinalTemp);

                                qDebug()<<"FinalTemp.size() = "<<FinalTemp.size();
                                string feature = string((char*)FinalTemp.data(),(FinalTemp.size() * sizeof(float)));
                                g_featureVtr.push_back(feature);

                                //
                                Global::instance()->stopProcessEnroll();
                                emit sigFeatureResult(6);

//                                g_sdkStatus = E_SDK_STATUS::e_suspend;
                            }
                        }
                        else if(g_sdkStatus & E_SDK_STATUS::e_match)
                        {
                            qDebug()<<"0000000000000000";
                            g_ovsInfo.is_feature_ok = true;

                            //
                            _mutex.lock();
                            g_matVtr.push_back(_StatusInfo.image_raw);
                            _mutex.unlock();
                            g_featureVtr.push_back(feature);
                            qDebug()<<"111111111111111111";
                        }
                    }
                    else
                    {
                        //                logde<<"exec false ------------------";
                        qDebug()<<"exec false ------------------  fTimerGetFeature = "<<fTimerGetFeature;
                        g_ovsInfo.iMatchImgNumFail ++;
                        g_iEnrollFeatureCount = 0;
                        g_ovsInfo.palm_position = OVSInformation::NoPalm;
                        g_ovsInfo.is_feature_ok = false;

                        if(g_sdkStatus & E_SDK_STATUS::e_match){
                            g_matVtr.clear();
                            g_featureVtr.clear();
                        }
                    }

                    if((true == g_bUpdateImage) && (g_sdkStatus & E_SDK_STATUS::e_match))
                    {
//                        qDebug()<<"send sigExecuteResult";
//                        g_ovsInfo.iDistance = dis;
//                        emit sigExecuteResult(_StatusInfo,g_ovsInfo,bSQLSaveImg);
                    }
                    //endof dis enable
                }
                else
                {
                    //
                    g_iEnrollFeatureCount = 0;
                    //
                    ++g_iMatchDisMark;
                    //
                    if(_enrollStageUpdateFlag){
                        preDisEnableTimes++;
                    }

                    g_ovsInfo.is_feature_ok = false;
                    g_ovsInfo.palm_cx = -1;
                    g_ovsInfo.palm_cy = -1;

                    if(g_sdkStatus & E_SDK_STATUS::e_match){
                        g_matVtr.clear();
                        g_featureVtr.clear();
                    }
                }
                qDebug()<<"2222222222222";
                QThread::msleep(conAlgorithmIntervalMs);
#endif
            }
            //        LogOut("ThreadDis slot_onThreadAlgoRun: " + QString::number(_thdAlgoRunFlag),LOG_DEBUG);
            LogOut("ThreadDis slot_onThreadAlgoRun stop",LOG_DEBUG);
        }



        ThreadImage::ThreadImage()
        {

        }

        ThreadImage *ThreadImage::instance()
        {
            static ThreadImage ins;
            return &ins;
        }

        ThreadImage::~ThreadImage()
        {

        }

        void ThreadImage::slot_onThreadImageRun()
        {
            QString str = "ThreadDis slot_onThreadImageRun";
            LogOut(str,LOG_DEBUG);

            while(_thdImageRunFlag)
            {
                //
                if(g_bUpdateImage)
                {
                    qDebug()<<"slot_onThreadImageRun 00";
                    _mutex.lock();
                    bool bread = g_cvCap.read(g_matRaw);
                    _mutex.unlock();
                    qDebug()<<"slot_onThreadImageRun 11";

                    if(bread)
                    {
                        if(g_matRaw.empty())
                            continue;

#if 1
                        cv::Mat ok_dst;
                        g_matRaw(cv::Range(0, 1080),
                                 cv::Range(420, 1500)).copyTo(ok_dst);

                        cv::Mat tmpMat;
                        cv::resize(ok_dst, tmpMat,
                                   cv::Size(100,100),
                                   0, 0, cv::INTER_NEAREST);

                        transpose(tmpMat, g_matPreview);
#endif
                        qDebug()<<"slot_onThreadImageRun 21";

                        //            Global::instance()->callBackGetPreviewImage();
                    }
                    qDebug()<<"slot_onThreadImageRun 22";

                    QThread::msleep(conImageIntervalMs);
                }
                else
                {
                    qDebug()<<"slot_onThreadImageRun 33";

                    if(!g_matRaw.empty())
                        g_matRaw.release();

                    if(!g_matPreview.empty())
                        g_matPreview.release();
                    qDebug()<<"slot_onThreadImageRun 44";

                    QThread::msleep(conImageIntervalMs);
                }


            }

            str = "ThreadDis slot_onThreadImageRun stop";
            LogOut(str,LOG_DEBUG);
        }

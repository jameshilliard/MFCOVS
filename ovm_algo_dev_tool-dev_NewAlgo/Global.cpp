#include "Global.h"
#include "MatchRealTime/ThreadHandler.h"

#include <QDir>
#include <QFile>
#include <iomanip>
#include <stdio.h>
#include <sstream>

int i = 0;

//test start
std::string strFeature1;
std::string strFeature2;
std::string strFeature3;
std::string strFeature4;
std::string strFeature5;
//test end

bool g_bUpdateImage = true;
int iHanming           = 0;
int g_i1V1Times        = 0;
int g_sensorDis        = -1;
int g_iMatchDisMark    = 0;
int g_moduleVideoNum   = -1;
int g_iDisSensorLow    = 40;
int g_iDisSensorLowH   = 60;
int g_iDisSensorLowH1  = 68;
int g_iDisSensorLowH2  = 76;
int g_iDisSensorHighL  = 85;
int g_iDisSensorHigh   = 115;
int g_iEnrollFeatureCount = 0;
int g_matchType           = MATCH_SDK_STATUS::e_null;
int g_sdkStatus           = E_SDK_STATUS::e_not_init;
MATCH_SDK_STATUS g_eSDKStatus = MATCH_SDK_STATUS::e_null;
MatchLevel g_eMatchLevel      = MatchLevel::lv_1;

QMutex _mutex;
std::map<std::string,int> g_map1vNTimes;
std::vector<std::string> g_featureVtr;
std::vector<cv::Mat> g_matVtr;
PalmImageInfo g_palmImageInfo;
OVSInformation g_ovsInfo;
cv::VideoCapture g_cvCap;
cv::Mat g_matRaw,g_matPreview;
MatchResultCount g_eMatchResultCount;
QList<cv::Mat> ListTimeoutMat;
QList<QString> ListTimeoutInfo;

Global::Global()
{
    QString FilePath = QDir::currentPath() + "/config.txt";

    QFile file(FilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open failed." << endl;
        return;
    }

    QByteArray byte = file.readAll();

    if(byte.indexOf("=") > 0)
    {
//        QList<QByteArray> HanmingList = byte.split("=");
        int iSize = byte.size();
        QByteArray bHanming = byte.mid((byte.indexOf("=") + 1),(iSize - byte.indexOf("=")));
        bHanming.replace(QString(";"), QByteArray(""));
        bHanming.replace(QString("\n"), QByteArray(""));
        iHanming = bHanming.toInt();

        qDebug()<<"iHanming = "<<iHanming<<",bHanming = "<<bHanming;
    }
}

Global *Global::instance()
{
    static Global ins;
    return &ins;
}

void Global::InitGlobalParam()
{
    memset(&g_eMatchResultCount,0,sizeof(MatchResultCount));
}

void Global::stopProcessEnroll()
{
    qDebug()<<"stopProcessEnroll start";
    g_ovsInfo.enroll_stage = e_algo_stage_1;
    g_ovsInfo.enroll_progress = 0;

    g_ovsInfo.is_feature_ok = true;
    //block all thread
    ThreadHandler::instance()->ThreadStop();
    //
    g_sensorDis = 0;
}

void Global::ResetMatchResultCount()
{

}

std::string Global::getMatchDisMarkStr()
{
    if(g_iMatchDisMark > 9999){
        g_iMatchDisMark = 0;
    }

    std::stringstream ss;
    ss<<std::setw(4) << std::setfill('0')<<g_iMatchDisMark;

    return ss.str();
}

int Global::getPreviewImageDetail(OVSImage &image, std::string &feature, OVSInformation &info)
{
//    if(false == Global::instance()->isDeviceWorking()){
//        return OVS::ERR_NO_DEVICE;
//    }
    //
//    if(E_SDK_STATUS::e_suspend == g_sdkStatus){
//        return OVS::STATUS::ERR_PROCESS_NOT_START;
//    }

//    //
//    if(g_matPreview.empty()){
//        return OVS:: ERR_PREVIEW_IS_EMPTY;
//    }

    //image
    image.dataVtr.clear();
    image.width = 0;
    image.height = 0;

    int dis = g_sensorDis;
//    qDebug()<<"55555555555555555";
    //    if((dis > g_iDisSensorLow) && (dis < g_iDisSensorHigh))
    if((dis > 0) && (dis < g_iDisSensorHigh))
    {
        if(false == g_matPreview.empty())
        {
            uchar *data = g_matPreview.data;
            image.dataVtr.clear();
            image.dataVtr.insert(image.dataVtr.end(),
                                 data,
                                 data + strlen((char*)data));
            image.width = g_matPreview.cols;
            image.height = g_matPreview.rows;
        }
    }
//    qDebug()<<"66666666666666";

    //feature
    feature = getFeature();
//    qDebug()<<"77777777777777";

    //info
    {
//        std::lock_guard<std::mutex> lkd(g_mtxOvsInfo);
        info = g_ovsInfo;

        g_ovsInfo.is_feature_ok = false;
    }

    return SUCCESS;
}

void Global::str2VtrTemplate(const std::string &str, Vtrf &vtr)
{
    if(str.empty()){
        return;
    }

    vtr.clear();
    vtr.resize(str.size() / sizeof(float));

    memcpy(vtr.data(),str.data(),str.size());
}

std::string Global::getFeature()
{
    std::string strFeature;
    if(g_ovsInfo.is_feature_ok){
        {
//            std::lock_guard<std::mutex> lkd(g_mtxFeature);

            if(E_SDK_STATUS::e_enroll & g_sdkStatus){

                if(g_featureVtr.size() > 0){

                    strFeature  = g_featureVtr[0];
#ifdef TESTCODE //test  同时添加不同规则的模板
                    strFeature1 = g_featureVtr[1];
                    strFeature2 = g_featureVtr[2];
                    strFeature3 = g_featureVtr[3];
                    strFeature4 = g_featureVtr[4];
                    strFeature5 = g_featureVtr[5];
#endif
                    g_featureVtr.clear();

//                    g_sdkStatus = E_SDK_STATUS::e_suspend;
                }
            }else if(E_SDK_STATUS::e_match & g_sdkStatus){
                if(g_featureVtr.size() > 0){
                    strFeature = g_featureVtr[0];
                    g_featureVtr.clear();
                }
            }
        }
        //
        if(g_sdkStatus & E_SDK_STATUS::e_enroll)
            Global::instance()->resetGlobalParams();

    }

    return strFeature;
}

void Global::resetGlobalParams()
{
//    g_sdkStatus = E_SDK_STATUS::e_suspend;

    g_ovsInfo.palm_position = OVSInformation::NoPalm;
    g_ovsInfo.palm_cx = -1;
    g_ovsInfo.palm_cy = -1;
    g_ovsInfo.enroll_stage = e_algo_stage_1;
    g_ovsInfo.enroll_progress = 0;

    g_featureVtr.clear();
}

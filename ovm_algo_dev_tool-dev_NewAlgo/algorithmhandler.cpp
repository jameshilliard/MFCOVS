#include "algorithmhandler.h"
#include "algorithm/include/aux_utils.h"
#include "algorithm/include/palm_vein.hpp"

#if defined(__linux__)      //add for windows
extern "C"{
#include <linux/videodev2.h>
}
#endif

#include "opencv2/opencv.hpp"

const int   iStandardHamming = 7;
const float conStandardHamming = 0.60f;

using namespace std;
using namespace cv;

AlgorithmHandler::AlgorithmHandler()
{
    pv       = NULL;
}

AlgorithmHandler::~AlgorithmHandler()
{
}

AlgorithmHandler *AlgorithmHandler::instance()
{
    static AlgorithmHandler ins;
    return &ins;
}


bool AlgorithmHandler::execute(const Mat &mat,PalmImageInfo &info,std::string &feature,
                               AuxDev::StatusInfo &_statusInfo)
{
    std::vector<float> dataVtr;

    bool re = execute(&mat,&info,dataVtr,_statusInfo);

    if(re){
        int byteLength = dataVtr.size() * sizeof(float);
        feature = string((char*)dataVtr.data(),byteLength);
    }

    return re;
}

bool AlgorithmHandler::execute(
        const Mat *raw,
        PalmImageInfo *sInfo,
        std::vector<float>& dataVtr,
        AuxDev::StatusInfo &_statusInfo)
{
//    OVS::PalmVein pv(raw->data, raw->cols, raw->rows, 3);

    if(pv == NULL)
    {
        qDebug()<<"AlgorithmHandler::execute pv == NULL";
        pv = new OVS::PalmVein(raw->data, raw->cols, raw->rows, 3);
    }
    else
    {
        qDebug()<<"AlgorithmHandler::execute pv != NULL";
        delete pv;
        pv = NULL;
        pv = new OVS::PalmVein(raw->data, raw->cols, raw->rows, 3);
    }


    if(pv == NULL)
        return false;

    if(pv->loadModelFiles("./palm_hog.yaml",
                         "./finger_hog_norm.yaml")){
//        qDebug()<<"load model file succ.";
    }else{
        qDebug()<<"load model file failed.";
//        logde<<"load model file failed.";

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }
        return e_err_load_file;
    }

    PalmImageInfo *info = sInfo;

    if(pv == NULL)
        return false;

    auto rlt = pv->parseInfo(*info);

    if (rlt != OVS_SUCCESS)
    {
//        logde<<"parseInfo failed.----------------------------info->score = "<<info->score;
//        qDebug()<<"parseInfo failed.---------------------------info->score = "<<info->score;
        info->center_x = -1;
        info->center_y = -1;
        info->score = -1;
        info->mean_illumination = -1;

        if(pv == NULL)
            return false;

        pv->loadStatusInfo();//每次调用

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }

        return e_err_parse_info;
    }else{
//        logde<<"parseInfo succ.++++++++++++++++++++++++++++++info->score = "<<info->score;
//        qDebug()<<"parseInfo succ.++++++++++++++++++++++++++++++info->score = "<<info->score;
    }

    if (info->mean_illumination < constIlluminationLow)
    {
        qDebug()<<"algo,mean_illumination:"<<info->mean_illumination;
//        logde<<"algo,mean_illumination:"<<info->mean_illumination;
        g_eMatchResultCount.RoiWeekNumber++;

        rlt = OVS_ERR_ROI_TOOWEAK;
//        logde<<"roi is too weak..----------------------------";
//        return (int)info->mean_illumination;
        if(pv == NULL)
            return false;

        pv->loadStatusInfo();//每次调用

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }
        return false;
    }
    else if (info->mean_illumination > constIlluminationHigh)
    {
        qDebug()<<"algo,mean_illumination:"<<info->mean_illumination;
//        logde<<"algo,mean_illumination:"<<info->mean_illumination;
        g_eMatchResultCount.RoiStrongNumber++;

        if(pv == NULL)
            return false;

        pv->loadStatusInfo();//每次调用
        rlt = OVS_ERR_ROI_TOOSTRONG;
//        logde<<"roi is too strong.---------------------------";
//        return (int)info->mean_illumination;

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }
        return false;
    }

    if(pv == NULL)
        return false;

    uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
    rlt = pv->findRoi(nullptr, roi_width, roi_height, roi_byte_size);

    if (rlt != OVS_SUCCESS)
    {
//        logde<<"fail to extract roi..---------------------------";
        g_eMatchResultCount.RoiPostionFailNumber++;

        if(pv == NULL)
            return false;

        pv->loadStatusInfo();//每次调用

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }
        return e_err_find_roi;
    }

    if(pv == NULL)
        return false;

    uint8_t *roi_data = new uint8_t[roi_byte_size];
    rlt = pv->findRoi(roi_data, roi_width, roi_height, roi_byte_size);

    if (rlt != OVS_SUCCESS)
    {
        delete[] roi_data;
//        logde<<"fail to extract roi.---------------------------.";

        if(pv == NULL)
            return false;

        pv->loadStatusInfo();//每次调用

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }
        return e_err_find_roi;
    }

    if(pv == NULL)
        return false;

    rlt = pv->computeFeature(dataVtr);

    //获取显示数据
    pv->loadStatusInfo();//每次调用
    extract_status(pv->_statusInfo);
    _statusInfo = pv->_statusInfo;

    if(rlt == OVS_SUCCESS)
    {
        delete[] roi_data;

        return e_succ;
#if 0
        for(const auto& f : feat_list)
        {
            auto hamming = OVS::PalmVein::compareFeature(&f[0], &feat[0], feat.size());
            cout << " " << hamming;
        }
        cout << endl;
        feat_list.push_back(feat);
#endif
    }else{
        delete[] roi_data;

        if(pv != NULL)
        {
            delete pv;
            pv = NULL;
        }
        return e_err_compute_feature;
    }
}

int AlgorithmHandler::extract_status(AuxDev::StatusInfo &info)
{
#if 0
    //A1 手掌检测
    {
        bool flag = info.a1_palm.success;
        float tm = info.a1_palm.time_consume;
    }

    //A2 指缝检测
    {
        bool flag = info.a2_finger.success;
        float tm = info.a2_finger.time_consume;
    }

    //A3 ROI检测
    {
        bool flag = info.a3_roi.success;
        float tm = info.a3_roi.time_consume;
    }

    //A4 预处理
    {
        bool flag = info.a4_enhance.success;
        float tm = info.a4_enhance.time_consume;
    }

    //A5 计算特征
    {
        bool flag = info.a5_fcompute.success;
        float tm = info.a5_fcompute.time_consume;
    }

#endif
    //显示图像
    Mat im_wnd;
    info.image_wnd.copyTo(im_wnd);
    Mat im_roi;
    info.image_roi.copyTo(im_roi);
    Mat im_svm;
    info.image_svm.copyTo(im_svm);

    return 1;
}

void AlgorithmHandler::ClearStatusInfo()
{
    if(pv == NULL)
        return;

    pv->clearStatusInfo();

//    if(pv != NULL)
//    {
//        delete pv;
//        pv = NULL;
//    }
}

AlgoMatch *AlgoMatch::instance()
{
    static AlgoMatch ins;
    return &ins;
}

AlgoMatch::AlgoMatch()
{

}

AlgoMatch::~AlgoMatch()
{

}

bool AlgoMatch::computeFeature(const float *src, const float *dst, uint32_t len, float &hammingValue)
{
    if((src == nullptr) || (dst == nullptr) ){
        return false;
    }

    float hamming = 0.0f;
    hamming       = OVS::PalmVein::compareFeature(src,dst,len);

    if(conStandardHamming >= hamming)
    {
//        qDebug()<<"hamming:"<<hamming;

        hammingValue = hamming;
        return true;
    }
    else
        return false;

}

bool AlgoMatch::computeFeature(const Vtrf &src, const Vtrf &dst, uint32_t len, float &hammingValue)
{
    qDebug()<<"测试比对速度流程";

    QMap<int,Vtrf> qMapTmp;
    QMap<int,Vtrf>::iterator it;

    for(int i = 0;i < 100000;i++)
    {
        qMapTmp.insert(i,src);
    }
    qDebug()<<"qMapTmp.size() = "<<qMapTmp.size();

    float hamming = 0.0f;
    QElapsedTimer         msTimer;     //一次完整时间
    msTimer.start();
    for(it = qMapTmp.begin();it != qMapTmp.end();it++)
    {
        hamming  = OVS::PalmVein::compareFeature((*it).data(),dst.data(),len);
    }
    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;
    qDebug()<<"computeFeature fTimerEnreoll = "<<fTimerEnreoll;


    if(conStandardHamming >= hamming)
    {
//        qDebug()<<"hamming:"<<hamming;

        hammingValue = hamming;
        return true;
    }
    else
        return false;
}

bool AlgoMatch::computeFeature(const Vtrf &src, const Vtrf &dst, int &SuccHamming, int &FailHamming)
{
    QMap<int,Vtrf>::iterator it;

    int hamming = 0;
//    QElapsedTimer         msTimer;     //一次完整时间
//    msTimer.start();

    Mat downsampled = Mat::zeros(10, 10, CV_8UC1);
    qDebug()<<"AlgoMatch::computeFeature downsampled.rows = "<<downsampled.rows<<",downsampled.cols = "<<downsampled.cols;
    OVS::PalmVein pv(downsampled.data, downsampled.rows, downsampled.cols, 3);
    hamming  = pv.compareFeature(src.data(),src.size(),dst.data(),dst.size());

//    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;
//    qDebug()<<"computeFeature fTimerEnreoll = "<<fTimerEnreoll;

    FailHamming = hamming;
    qDebug()<<"hamming:"<<hamming<<",iHanming = "<<iHanming;

    if(iHanming > 0)
    {
        if(hamming >= iHanming)
        {
            SuccHamming = hamming;
            return true;
        }
        else
            return false;
    }
    else
    {
        if(hamming >= iStandardHamming)
        {
            SuccHamming = hamming;
            return true;
        }
        else
            return false;
    }

}

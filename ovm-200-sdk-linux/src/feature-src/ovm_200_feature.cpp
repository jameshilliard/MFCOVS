#include "ovm_200_feature.h"
#include "global.h"
#include "logger.h"
#include "opencv/cv.h"
#include "threadhandler.h"
#include "devhandler.h"

const int conExposureDefault = 28;
const int conLedCurrentDefault = 0;

using namespace std;

OVS::STATUS OVS::InitFeatureSDK()
{
    //
    if(E_SDK_STATUS::e_not_init != g_sdkStatus){
        return OVS::ERR_SDK_ALREADY_INITIALIZED;
    }
    //
    Logger::instance();
    //
    Global::instance()->resetGlobalParams();
    //
    g_moduleVideoNum = Global::instance()->getModuleVideoNum();
    if(g_moduleVideoNum == -1){
        return OVS::ERR_NO_DEVICE;
    }
    //
    DevHandler *devh = DevHandler::instance();
    if(false == devh->init()){
        return OVS::ERR_DEV_OPEN_FAILED;
    }
    //
#if defined(__linux__)      //add for windows
    g_cvCap = cv::VideoCapture(g_moduleVideoNum);
#else
    g_cvCap = cv::VideoCapture(g_moduleVideoNum + cv::CAP_DSHOW);
#endif

    g_cvCap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    g_cvCap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    if(g_cvCap.isOpened()){
        logde<<"cap opened succ.";
    }else{
        logde<<"cap open failed.";
        return OVS::ERR_SDK_INIT_FAIL;
    }
    //
    devh->setExposure(conExposureDefault);
    devh->setLEDCurrent(conLedCurrentDefault);
    //
    ThreadHandler::instance();
    //
    return OVS::SUCCESS;
}

OVS::STATUS OVS::UninitFeatureSDK()
{
    //
    if(false == Global::instance()->isDeviceWorking()){
        ThreadHandler::instance()->blockAllThd();
        return OVS::ERR_NO_DEVICE;
    }
    //检查sdk当前状态
    if(E_SDK_STATUS::e_suspend != g_sdkStatus){
        if(E_SDK_STATUS::e_enroll & g_sdkStatus){
            return OVS::ERR_ENROLL_PROCESS_NOT_STOP;
        }else if(E_SDK_STATUS::e_match & g_sdkStatus){
            return OVS::ERR_MATCH_PROCESS_NOT_STOP;
        }
    }
    //
    g_cvCap.release();
    //
    if(false == DevHandler::instance()->unInit()){
        return OVS::ERR_SDK_UNINIT_FAIL;
    }
    //
    Global::instance()->resetGlobalParams();
    //
    ThreadHandler::instance()->blockAllThd();
    //
    return OVS::SUCCESS;
}

OVS::STATUS OVS::GetPreviewImage(OVS::Image &image,
                                 string &feature,
                                 OVS::Information &info)
{
    return Global::instance()->getPreviewImageDetail(
                image,feature,info);
}

OVS::STATUS OVS::AbortCapture()
{
    //
    if(false == Global::instance()->isDeviceWorking()){
        ThreadHandler::instance()->blockAllThd();
        return OVS::ERR_NO_DEVICE;
    }

    Global::instance()->resetGlobalParams();

    ThreadHandler::instance()->blockDisThread();
    ThreadHandler::instance()->blockImageThread();
    ThreadHandler::instance()->blockAlgoThread();
    
    return OVS::SUCCESS;
}

#if 0
OVS::STATUS OVS::StartFeatureForMatch(
        OVS::OVSMatchGetPreviewCallback previewCB,
        OVS::OVSMatchGetFeatureCallback featrueCB)
{
    if(E_SDK_STATUS::e_suspend != g_sdkStatus){
        return ERR_ENROLL_PROCESS_NOT_STOP;
    }
    //比对过程只能使用回调的方式来获取图像和特征信息.
    if((previewCB == nullptr) || (featrueCB == nullptr)){
        return ERR_ENROLL_PROCESS_NOT_STOP;
    }else{
        g_pMatchGetPreviewCallback = previewCB;
        g_pMatchGetFeatureCallback = previewCB;

        g_sdkStatus = E_SDK_STATUS::e_match
                | E_SDK_STATUS::e_func_mode_callback;
    }

    ThreadHandler::instance()->notifyDisThread();
    ThreadHandler::instance()->notifyImageThread();
    ThreadHandler::instance()->notifyAlgoThread();

    return OVS::SUCCESS;
}
#endif

#if 0
OVS::STATUS OVS::StartFeatureForEnroll(OVSEnrollGetPreviewCallback call)
{
    if(E_SDK_STATUS::e_suspend != g_sdkStatus){
        if(E_SDK_STATUS::e_enroll & g_sdkStatus){
            return OVS::ERR_ENROLL_PROCESS_NOT_STOP;
        }else if(E_SDK_STATUS::e_match & g_sdkStatus){
            return OVS::ERR_MATCH_PROCESS_NOT_STOP;
        }
    }

    if(call == nullptr){
        g_sdkStatus = E_SDK_STATUS::e_enroll
                | E_SDK_STATUS::e_func_mode_normal_func;
    }else{
        g_pEnrollGetPreviewCallback = call;
        g_sdkStatus = E_SDK_STATUS::e_enroll
                | E_SDK_STATUS::e_func_mode_callback;
    }

    ThreadHandler::instance()->notifyDisThread();
    ThreadHandler::instance()->notifyImageThread();
    ThreadHandler::instance()->notifyAlgoThread();

    return OVS::SUCCESS;
}
#endif

OVS::STATUS OVS::StartEnrollMode(
        OVS::OVSGetPreviewCallback previewCB,
        OVS::OVSGetFeatureCallback featureCB)
{
    return Global::instance()->startFeature(
                E_SDK_STATUS::e_enroll,
                previewCB,
                featureCB);
}

OVS::STATUS OVS::StartMatchMode(
        OVS::OVSGetPreviewCallback previewCB,
        OVS::OVSGetFeatureCallback featureCB)
{
    return Global::instance()->startFeature(
                E_SDK_STATUS::e_match,
                previewCB,
                featureCB);
}

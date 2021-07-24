#include "global.h"
#include "logger.h"
#include "threadhandler.h"

#include <stdio.h>
#include <mutex>
#include <sstream>
#include <numeric> 

/*************************************************************/
//
/*************************************************************/
//global values
int g_iDisSensorLow = 25;
int g_iDisSensorHigh = 110;

int g_iFeatureMax = 3;

OVS::OVSGetPreviewCallback g_pGetPreviewCallback = nullptr;
OVS::OVSGetFeatureCallback g_pGetFeatureCallback = nullptr;

int g_moduleVideoNum = -1;

int g_sdkStatus = E_SDK_STATUS::e_not_init;

cv::VideoCapture g_cvCap;

cv::Mat g_matRaw,g_matPreview;
std::mutex g_mtxMatRaw,g_mtxMatAlgo,g_mtxMatPreview;

std::mutex g_mtxFeature;

OVS::Information g_ovsInfo;
std::mutex g_mtxOvsInfo;

int g_sensorDis;

PalmImageInfo g_palmImageInfo;

std::vector<std::string> g_featureVtr;

int g_iEnrollFeatureCount = 0;
//
int g_iFlashReReadTimes = 5;
///match
int g_iMatchDisMark = 0;

/*************************************************************/
mutex Global::_mtxFet;
condition_variable Global::_condFet;
bool Global::_bFlagThdFetRun = true;
bool Global::_bFlagThdFetBlock = true;


Global::Global()
{
    //    _thdFet = thread(_funcThdFet);
    //    _thdFet.detach();
}

Global *Global::instance()
{
    static Global ins;
    return &ins;
}

void Global::resetGlobalParams()
{
    g_sdkStatus = E_SDK_STATUS::e_suspend;

    g_ovsInfo.palm_position = OVS::Information::NoPalm;
    g_ovsInfo.palm_cx = -1;
    g_ovsInfo.palm_cy = -1;
    g_ovsInfo.enroll_stage = e_algo_stage_1;
    g_ovsInfo.enroll_progress = 0;

    g_featureVtr.clear();
}

#if defined(__linux__)      //add for windows
bool Global::getModuleVideoNumSub(const char *pcmd, string &sData){
    if(pcmd == NULL){
        return false;
    }

    FILE *fp = NULL;

    fp = popen(pcmd, "r");
    if (fp == NULL)
    {
        logde<<"popen error!";
        return false;
    }

    const int conArraySize = 50;
    char buff[conArraySize];
    bzero(buff,conArraySize);

    while (fgets(buff, conArraySize, fp) != NULL)
    {
        sData.append(string(buff));
    }

    pclose(fp);

    return true;
}
#endif

bool Global::isDeviceWorking()
{
#if defined(__linux__)
    std::ostringstream oss;
    oss<<"udevadm info -q property -n /dev/video"
      <<g_moduleVideoNum;
    string sCmd = oss.str();

    string sRe;
    getModuleVideoNumSub(sCmd.data(), sRe);

    //    device node not found
    if(sRe.empty()){
        return false;
    }else{
        return true;
    }
#else //add for windows
    return true;
#endif
}

OVS::STATUS Global::startFeature(const E_SDK_STATUS eStatus,
                                 OVS::OVSGetPreviewCallback previewCB,
                                 OVS::OVSGetFeatureCallback featrueCB)
{
    //
    if(false == Global::instance()->isDeviceWorking()){
        ThreadHandler::instance()->blockAllThd();
        return OVS::ERR_NO_DEVICE;
    }
    //检查参数
    if((previewCB == nullptr) && (featrueCB != nullptr)){
        return OVS::STATUS::ERR_INVALID_PARAM;
    }
    if((previewCB != nullptr) && (featrueCB == nullptr)){
        return OVS::STATUS::ERR_INVALID_PARAM;
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
    resetGlobalParams();
    //
    if(eStatus == E_SDK_STATUS::e_enroll){
        g_sdkStatus = E_SDK_STATUS::e_enroll;
    }else if(eStatus == E_SDK_STATUS::e_match){
        g_sdkStatus = E_SDK_STATUS::e_match;
    }
    //
    if((previewCB == nullptr) && (featrueCB == nullptr)){
        g_sdkStatus |= E_SDK_STATUS::e_func_mode_normal_func;
    }else{
        g_sdkStatus |= E_SDK_STATUS::e_func_mode_callback;
    }

    //
    g_pGetPreviewCallback = previewCB;
    g_pGetFeatureCallback = featrueCB;
    //
    ThreadHandler::instance()->notifyDisThread();
    ThreadHandler::instance()->notifyImageThread();
    ThreadHandler::instance()->notifyAlgoThread();
    //
    return OVS::SUCCESS;
}

OVS::STATUS Global::getPreviewImageDetail(
        OVS::Image &image,
        string &feature,OVS::Information &info)
{
    //
    if(false == Global::instance()->isDeviceWorking()){
        return OVS::ERR_NO_DEVICE;
    }
    //
    if(E_SDK_STATUS::e_suspend == g_sdkStatus){
        return OVS::STATUS::ERR_PROCESS_NOT_START;
    }

    //
    if(g_matPreview.empty()){
        return OVS:: ERR_PREVIEW_IS_EMPTY;
    }

    //image
    image.dataVtr.clear();
    image.width = 0;
    image.height = 0;

    int dis = g_sensorDis;
    if((dis > g_iDisSensorLow) && (dis < g_iDisSensorHigh))
    {
        if(false == g_matPreview.empty()){
#if 1
            uchar *data = g_matPreview.data;
            image.dataVtr.clear();
            image.dataVtr.insert(image.dataVtr.end(),
                                 data,
                                 data + strlen((char*)data));
#endif
            image.width = g_matPreview.cols;
            image.height = g_matPreview.rows;
#if 0
            uchar *data = g_matPreview.data;
            image.length = strlen((char*)g_matPreview.data);
            image.data = (uchar*)malloc(image.length);
            memcpy(image.data,data,image.length);
#endif
        }
    }


    //feature
    feature = getFeature();

    //info
    {
        std::lock_guard<std::mutex> lkd(g_mtxOvsInfo);
        info = g_ovsInfo;

        g_ovsInfo.is_feature_ok = false;
    }

    return OVS::SUCCESS;
}

void Global::callBackGetPreviewImage()
{
    if(E_SDK_STATUS::e_func_mode_callback & g_sdkStatus){
        if(g_pGetPreviewCallback == nullptr){
            logde<<"call back is null.";
            return;
        }

        if(*g_pGetPreviewCallback == nullptr){
            logde<<"call back ptr is null.";
            return;
        }

        OVS::Image tmpOVSImage;
        string sFeature;
        OVS::Information tmpOVSInfo;

        OVS::STATUS st = getPreviewImageDetail(
                    tmpOVSImage,sFeature,tmpOVSInfo);

        g_pGetPreviewCallback(st,tmpOVSImage,tmpOVSInfo);

        if(tmpOVSInfo.is_feature_ok){
            Global::instance()->thdFetNotify();
        }
    }
}

string Global::getMatchDisMarkStr()
{
    if(g_iMatchDisMark > 9999){
        g_iMatchDisMark = 0;
    }

    stringstream ss;
    ss<<setw(4) << setfill('0')<<g_iMatchDisMark;

    return ss.str();

#if 0
    logde<<__func__<<" dis mark:"<<g_iMatchDisMark;
    char chArray[4] = {'0'};
    sprintf(chArray,"%04d",g_iMatchDisMark);

    logde<<__func__;
    return string(chArray,conMatchDisMarkStrSize);
    logde<<__func__;

    //    string s(chArray,conMatchDisMarkStrSize);

    //    return s;
    //    return string();
#endif
}

void Global::thdFetBlock()
{
    _bFlagThdFetBlock = true;
}

void Global::thdFetNotify()
{
    _bFlagThdFetBlock = false;
    _condFet.notify_one();
}

void Global::stopProcessEnroll()
{
    //
    g_ovsInfo.enroll_stage = e_algo_stage_1;
    g_ovsInfo.enroll_progress = 0;

    g_ovsInfo.is_feature_ok = true;
    //block all thread
    ThreadHandler::instance()->blockAllThd();
    //
    g_sensorDis = 0;
}

string Global::getFeature()
{
    string strFeature;
    if(g_ovsInfo.is_feature_ok){
        {
            std::lock_guard<std::mutex> lkd(g_mtxFeature);

            if(E_SDK_STATUS::e_enroll & g_sdkStatus){

                if(g_featureVtr.size() > 0){

                    //display
#if 0
                    for(int i = 0; i < g_featureVtr.size(); i++){
                        logde<<"sdk,feature,feature data:"
                            <<i<<"|"<<g_featureVtr.at(i);
                    }
#endif
                    //


                    strFeature = std::accumulate(
                                g_featureVtr.begin(),
                                g_featureVtr.begin() + conIEnrollFeatureCount,
                                string(""));

                    g_featureVtr.clear();

                    g_sdkStatus = E_SDK_STATUS::e_suspend;
                }
            }else if(E_SDK_STATUS::e_match & g_sdkStatus){
                if(g_featureVtr.size() > 0){
                    strFeature = g_featureVtr[0];
                    g_featureVtr.clear();
                }
            }
        }
        //
        if(g_sdkStatus & E_SDK_STATUS::e_enroll){
            Global::instance()->resetGlobalParams();
        }
    }

    return strFeature;
}

void Global::_funcThdFet()
{
    while(_bFlagThdFetRun){
        if(_bFlagThdFetBlock){
            unique_lock<mutex> ulk(_mtxFet);
            _condFet.wait(ulk);
        }

        string strFeature = getFeature();

        if(g_pGetFeatureCallback != nullptr){
            g_pGetFeatureCallback(OVS::SUCCESS,strFeature);
        }

        Global::instance()->thdFetBlock();
    }
}

int Global::getModuleVideoNum()
{
#if defined(__linux__)      //add for windows
    string consCmdVid = "udevadm info -q property -n /dev/video%d | grep ID_VENDOR_ID";
    string consCmdPid = "udevadm info -q property -n /dev/video%d | grep ID_MODEL_ID";

    const int conCheckTimes = 99;
    const int conArrayCmdSize = 70;
    //
    char chCmdArray[conArrayCmdSize];

    for (int i = 0; i < conCheckTimes; i++) {
        string sVid;
        string sPid;
        //vid
        bzero(chCmdArray, conArrayCmdSize);
        sprintf(chCmdArray, consCmdVid.data(), i);
        if (false == getModuleVideoNumSub(chCmdArray, sVid)) {
            logde<<"get failed.";
            continue;
        }

        string::size_type sstVid = sVid.find(consModuleVid);
        if (string::npos == sstVid) {
            continue;
        }
        else {
            //pid
            bzero(chCmdArray, conArrayCmdSize);
            sprintf(chCmdArray, consCmdPid.data(), i);
            getModuleVideoNumSub(chCmdArray, sPid);

            string::size_type sstPid = sPid.find(consModulePid);
            if (string::npos != sstPid) {
                return i;
            }
        }
    }

    return -1;

# else      //add for windows
    vector<string> capture_list;
    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;
    int deviceCounter = 0;
    CoInitialize(NULL);

    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                  CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                  reinterpret_cast<void**>(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the video capture category.
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        if (hr == S_OK)
        {
            IMoniker* pMoniker = NULL;
            while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
            {
                IPropertyBag* pPropBag;
                hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
                if (FAILED(hr))
                {
                    pMoniker->Release();
                    continue;
                }
                // Find the description or friendly name.
                VARIANT varName;
                VariantInit(&varName);
                hr = pPropBag->Read(L"Description", &varName, 0);

                if (FAILED(hr))
                {
                    hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                }
                if (SUCCEEDED(hr))
                {
                    hr = pPropBag->Read(L"FriendlyName", &varName, 0);

                    int count = 0;
                    char tmp[255] = { 0 };
                    while (varName.bstrVal[count] != 0x00 && count < 255)
                    {
                        tmp[count] = (char)varName.bstrVal[count];
                        count++;
                    }
                    capture_list.push_back(tmp);
                }

                pPropBag->Release();
                pPropBag = NULL;

                pMoniker->Release();
                pMoniker = NULL;

                deviceCounter++;
            }

            pDevEnum->Release();
            pDevEnum = NULL;

            pEnum->Release();
            pEnum = NULL;
        }
    }
    //return deviceCounter;

    // 获取摄像头ID
    int capture_id = 0;
    const string omec_device = "OMEC Vein Scanner";
    for (int i = 0; i < capture_list.size(); ++i)
    {
        if (capture_list[i] == omec_device)
        {
            capture_id = i;
            break;
        }
    }
    return capture_id;
#endif
}

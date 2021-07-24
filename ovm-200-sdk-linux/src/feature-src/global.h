#ifndef GLOBAL_H
#define GLOBAL_H

#include "opencv2/opencv.hpp"
#include "utils.hpp"
#include "ovm_200_feature.h"
#include "utils.hpp"

#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

#if defined(__linux__)      //add for windows
//skip
#else
#include <dshow.h>        // std::accumulate
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")
#endif

using namespace std;
using namespace cv;

/*************************************************************/
//global defines

enum E_SDK_STATUS{
    e_not_init              = -1,
    e_suspend               = 0,
    e_enroll                = 0x01,
    e_match                 = 0x02,

    e_func_mode_callback    = 0x04,
    e_func_mode_normal_func = 0x08
};

struct StFeature{
    std::string name;
    std::string feature;
};

enum E_ALGO_RETURN_VALUE{
    e_succ                =     true,
    e_err_load_file       =     false,
    e_err_parse_info      =     false,
    e_err_find_roi        =     false,
    e_err_compute_feature =     false
};


const float constIlluminationLow = 70.0;
const float constIlluminationHigh = 200.0;

const int constIlluminationStep = 3;

const int conMatchDisMarkStrSize = 4;

const string consModuleVid("30d5");
const string consModulePid("2001");

//timer interval
const int conDisIntervaleMs      = 100;
const int conImageIntervalMs     = 30;
const int conAlgorithmIntervalMs = 100;
//
const int conPreDisEnableTimes   = 3;
const int conIEnrollFeatureCount = 5;
const int conTemplateCellFloatSize = 576;
/*************************************************************/
//global values
extern int g_iDisSensorLow;
extern int g_iDisSensorHigh;

extern int g_iFeatureMax;

extern OVS::OVSGetPreviewCallback g_pGetPreviewCallback;
extern OVS::OVSGetFeatureCallback g_pGetFeatureCallback;

extern int g_moduleVideoNum;

extern int g_sdkStatus;

extern cv::VideoCapture g_cvCap;

extern cv::Mat g_matRaw,g_matPreview;
extern std::mutex g_mtxMatRaw,g_mtxMatAlgo,g_mtxMatPreview;

extern std::mutex g_mtxFeature;

extern OVS::Information g_ovsInfo;
extern std::mutex g_mtxOvsInfo;

extern int g_sensorDis;

extern PalmImageInfo g_palmImageInfo;

enum E_ALGO_STATE{
    e_algo_stage_1    = 1,
    e_algo_stage_2    = 2,
    e_algo_stage_3    = 3 };

extern std::vector<std::string> g_featureVtr;

extern int g_iEnrollFeatureCount;
//
extern int g_iFlashReReadTimes;
///match feature
extern int g_iMatchDisMark;
/*************************************************************/

/**
 * @brief The Global class
 */
class Global
{
public:
    explicit Global();
    static Global *instance();
    //
    void initGlobalParams(){resetGlobalParams();}
    void resetGlobalParams();
    //device status
    int getModuleVideoNum();

#if defined(__linux__)
    bool getModuleVideoNumSub(const char*pcmd,string &sData);
#endif

    bool isDeviceWorking();

    //
    OVS::STATUS startFeature(const E_SDK_STATUS eStatus,
                             OVS::OVSGetPreviewCallback previewCB,
                             OVS::OVSGetFeatureCallback featrueCB);
    //
    OVS::STATUS getPreviewImageDetail(OVS::Image &image,
                                      string &feature,
                                      OVS::Information &info);

    //
    string getMatchDisMarkStr();
    //call back
    void callBackGetPreviewImage();
    //
    void thdFetBlock();
    void thdFetNotify();
    //
    void stopProcessEnroll();

private:
    static string getFeature();
    //
    thread _thdFet;
    static void _funcThdFet();
    static mutex _mtxFet;
    static condition_variable _condFet;
    static bool _bFlagThdFetRun;
    static bool _bFlagThdFetBlock;
    //

};



#endif // GLOBAL_H

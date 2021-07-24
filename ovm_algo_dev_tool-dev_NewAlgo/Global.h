#ifndef GLOBAL_H
#define GLOBAL_H

#define DebugInfo

#include <QMutex>
#include <QDebug>
#include <vector>
#include <QElapsedTimer>

#include "opencv2/opencv.hpp"
#include "algorithm/include/utils.hpp"
#include "algorithm/include/aux_utils.h"

//#define TESTCODE

typedef std::vector<float> Vtrf;
typedef std::vector<std::vector<float> > vvtrf;

enum OVSSTATUS
{
    SUCCESS                     = 0,      //函数操作成功
    ERR                         = -1,     //函数运行失败
    ERR_INVALID_PARAM           = -2,     //参数无效

    ERR_DEV_OPEN_FAILED         = -99,    //设备打开失败
    ERR_NO_DEVICE               = -100,   //设备不存在
    ERR_INVALID_DEVICE_INDEX    = -101,   //设备编号不存在
    ERR_NOT_REGISTERD_CALLBACK  = -103,   //未注册回调函数

    ERR_SDK_UNINITIALIZED       = -10,    //SDK未初始化
    ERR_SDK_ALREADY_INITIALIZED = -11,    //SDK已经初始化
    ERR_SDK_INIT_FAIL           = -12,    //SDK初始化失败
    ERR_SDK_UNINIT_FAIL         = -13,    //SDK uninit失败

    ERR_PROCESS_NOT_START       = -50,     //操作未启动
    ERR_ENROLL_PROCESS_NOT_START= -53,     //录入过程未启动
    ERR_ENROLL_PROCESS_NOT_STOP = -54,     //录入过程未停止
    ERR_MATCH_PROCESS_NOT_START = -55,     //比对过程未启动
    ERR_MATCH_PROCESS_NOT_STOP  = -56,     //比对过程未停止

    ERR_FUNC_MODE_CALL_BACK     = -199,   //已启用回调函数方式获取图像
    ERR_CAPTURE_DISABLED        = -200,   //图像采集未被使能
    ERR_CAPTURE_ABORTED         = -201,   //图像采集被终止
    ERR_CAPTURE_IS_RUNNING      = -202,   //图像采集正在运行
    ERR_CAPTURE_TIMEOUT         = -203,   //图像采集超时
    ERR_PREVIEW_IS_EMPTY        = -204,   //预览图像为空

    ERR_GET_FEATURE             = -300,   //提取特征点失败

    ERR_MATCH_FAILED            = -400,   //比对失败
    //match
    ERR_INSUFFICIENT_MEMORY     = -450,   //内存不足
    ERR_USER_ID_NOT_EXIST       = -451,   //用户ID未添加
    ERR_USER_ID_EXISTED         = -452,   //用户ID已经存在
    ERR_USER_LEFT_FEATURE_EXISTED      = -453,   //该用户左手特征数据已经存在
    ERR_USER_RIGHT_FEATURE_EXISTED     = -454,   //该用户右手特征数据已经存在

    ERR_USER_COUNT_ZERO         = -500,   //用户数量为0
    ERR_FAILED_TO_ADD_USER      = -501,   //添加用户失败
    ERR_FAILED_TO_UPDATE_USER   = -502,   //更新用户失败
    ERR_FAILED_TO_DELETE_USER   = -503    //删除用户失败

};

enum MatchMode{
    e_left,
    e_right,
    e_left_right
};

enum E_ALGO_RETURN_VALUE{
    e_succ                =     true,
    e_err_load_file       =     false,
    e_err_parse_info      =     false,
    e_err_find_roi        =     false,
    e_err_compute_feature =     false
};

enum E_ALGO_STATE{
    e_algo_stage_1    = 1,
    e_algo_stage_2    = 2,
    e_algo_stage_3    = 3
};

enum E_SDK_STATUS{
    e_not_init              = -1,
    e_suspend               = 0,
    e_enroll                = 0x01,
    e_match                 = 0x02,

    e_func_mode_callback    = 0x04,
    e_func_mode_normal_func = 0x08
};

enum MATCH_SDK_STATUS{
    e_null,
    e_1v1,
    e_1vN
};

enum MatchLevel{
    lv_1 = 1,    //
    lv_2,
    lv_3,
    lv_4,
    lv_5     //FAR min
};

struct OVSImage
{
    std::vector<unsigned char> dataVtr;
    int width;
    int height;
};

struct OVSInformation
{
    enum PalmPosition
    {
        NoPalm,
        Suitable,
        TooClose,
        TooFar,
        TooLeft,
        TooRight,
        TooUp,
        TooDown
    };

    PalmPosition palm_position; //手掌位置信息
    float palm_cx;                //手掌中心X坐标
    float palm_cy;                //手掌中心Y坐标

    bool is_feature_ok;  	    //判断是否可以提取特征点
    int enroll_stage;           //录入阶段，仅录入过程有效
    int enroll_progress;        //录入进度，仅录入过程有效
    int iDistance;

    int iMatchImgNumSucc;
    int iMatchImgNumFail;

    OVSInformation()
        :palm_position(PalmPosition::NoPalm)
        ,palm_cx(-1)
        ,palm_cy(-1)
        ,is_feature_ok(false)
        ,enroll_stage(1)
        ,enroll_progress(0){}
};

struct MatchResultCount
{
    //图片的总数量
    int ImageTotal;
    //未发现手掌的数量
    int NoFindPalmNumber;
    //未发现指缝的数量
    int NoFindFingerNumber;
    //其他问题的数量
    int OtherProblemNumber;
    //ROI区域太暗的数量
    int RoiWeekNumber;
    //ROI区域太亮的数量
    int RoiStrongNumber;
    //获取ROI失败的数量
    int GetRoiFailNumber;
    //ROI定位失败的数量
    int RoiPostionFailNumber;
    //获取特征数据失败的数量
    int GetFeatureFailNumber;
    //获取特征数据成功的数量
    int GetFeatureSuccessNumber;
};

extern bool g_bUpdateImage;// true:更新图像（代表match或enroll没有成功）      false：不更新图像
extern int iHanming;
extern int g_i1V1Times;
extern int g_sdkStatus;//程序运行状态
extern int g_matchType;
extern int g_sensorDis;
extern int g_iMatchDisMark;
extern int g_moduleVideoNum;
extern int g_iDisSensorLow;
extern int g_iDisSensorHigh;
extern int g_iDisSensorLowH;
extern int g_iDisSensorLowH1;
extern int g_iDisSensorLowH2;
extern int g_iDisSensorHighL;
extern int g_iEnrollFeatureCount;
extern std::vector<std::string> g_featureVtr;
extern std::vector<cv::Mat> g_matVtr;
extern PalmImageInfo g_palmImageInfo;
extern OVSInformation g_ovsInfo;
extern cv::VideoCapture g_cvCap;
extern cv::Mat g_matRaw,g_matPreview;
extern std::map<std::string,int> g_map1vNTimes;
extern MATCH_SDK_STATUS g_eSDKStatus;
extern MatchLevel g_eMatchLevel;
extern MatchResultCount g_eMatchResultCount;
extern QMutex _mutex;
extern QList<cv::Mat> ListTimeoutMat;
extern QList<QString> ListTimeoutInfo;

//test start
extern std::string strFeature1;
extern std::string strFeature2;
extern std::string strFeature3;
extern std::string strFeature4;
extern std::string strFeature5;
//test end

const int conIEnrollFeatureCount  = 5;
const int   conPreDisEnableTimes  = 3;
const float constIlluminationLow  = 70.0;
const float constIlluminationHigh = 200.0;

class Global
{
public:
    Global();
    static Global *instance();

    void InitGlobalParam();
    void stopProcessEnroll();
    void ResetMatchResultCount();
    std::string getMatchDisMarkStr();

    int getPreviewImageDetail(OVSImage &image,std::string &feature,OVSInformation &info);
    void str2VtrTemplate(const std::string& str,Vtrf&vtr);

private:
    void resetGlobalParams();
    static std::string getFeature();
};

#endif // GLOBAL_H

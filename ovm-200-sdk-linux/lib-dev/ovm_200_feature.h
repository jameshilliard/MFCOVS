#ifndef OVS_SDK_H
#define OVS_SDK_H

#include <string>
#include <vector>
#include "../ovm_200_defines.h"

#ifdef DWIN32
#ifdef DLL_EXPORTS
#ifdef DLL_INSIDE
#define OVS_EXPORT __declspec(dllexport)
#else
#define OVS_EXPORT __declspec(dllimport)
#endif
#else
#define OVS_EXPORT
#endif
#else
#define OVS_EXPORT
#endif

using namespace std;

namespace OVS{
/**
 * @brief The Image struct
 */
struct Image
{
    vector<unsigned char> dataVtr;
    unsigned char *data;
    int length;

    int width;
    int height;
};
/**
 * @brief The Informatinon struct
 */
struct Information
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
    int palm_cx;                //手掌中心X坐标
    int palm_cy;                //手掌中心Y坐标

    bool is_feature_ok;  	    //判断是否可以提取特征点
    int enroll_stage;           //录入阶段，仅录入过程有效
    int enroll_progress;        //录入进度，仅录入过程有效

    Information()
        :palm_position(PalmPosition::NoPalm)
        ,palm_cx(-1)
        ,palm_cy(-1)
        ,is_feature_ok(false)
        ,enroll_stage(1)
        ,enroll_progress(0){}
};

STATUS InitFeatureSDK();

STATUS UninitFeatureSDK();

//match
//比对过程只能使用回调的方式来获取图像和特征信息.
typedef void (*OVSGetPreviewCallback)(
        OVS::STATUS result,OVS::Image &image,OVS::Information &info);

typedef void(*OVSGetFeatureCallback)(
        OVS::STATUS result,string &feature);

STATUS StartEnrollMode(OVSGetPreviewCallback previewCB,
                       OVSGetFeatureCallback featureCB);
STATUS StartMatchMode(OVSGetPreviewCallback previewCB,
                      OVSGetFeatureCallback featureCB);

STATUS GetPreviewImage(OVS::Image& image,string &feature,OVS::Information& info);


STATUS AbortCapture();

}
#endif

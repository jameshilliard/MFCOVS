#ifndef OVS_SDK_H
#define OVS_SDK_H

#include <string>
#include <vector>
#include "ovm_200_defines.h"

#ifdef WIN32
#include <atlimage.h>
#if defined(__AFXSTR_H__) || defined(__ATLSTR_H__)
#include <atlstr.h>
#endif //defined(__AFXSTR_H__) || defined(__ATLSTR_H__)
#ifdef ovm_200_feature_EXPORTS
#define OVMSDK_API __declspec(dllexport) 
#else //ovm_200_feature_EXPORTS
#define OVMSDK_API __declspec(dllimport) 
#endif //ovm_200_feature_EXPORTS
#endif //WIN32


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
        NoPalm,       //OVS_ERR_NO_FOUND_PALM
        Suitable,
        Unsuitable,   //手掌位置不对  OVS_ERR_NO_FOUND_FINGER
                                  //OVS_ERR_GET_ROI_FAILED
                        //OVS_ERR_GET_FEATURE_FAILED
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

    bool is_feature_ok;  	//判断是否可以提取特征点
    int enroll_stage;           //录入阶段，仅录入过程有效
    int enroll_progress;        //录入进度，仅录入过程有效

    Information()
        :palm_position(PalmPosition::NoPalm)
        ,palm_cx(-1.0f)
        ,palm_cy(-1.0f)
        ,is_feature_ok(false)
        ,enroll_stage(1)
        ,enroll_progress(0){}
};

OVMSDK_API STATUS __stdcall InitFeatureSDK();

OVMSDK_API STATUS __stdcall UninitFeatureSDK();

//match
//比对过程只能使用回调的方式来获取图像和特征信息.
typedef void (__stdcall *OVSGetPreviewCallback)(
        OVS::STATUS result,OVS::Image &image,OVS::Information &info);

typedef void(__stdcall *OVSGetFeatureCallback)(
        OVS::STATUS result,string &feature);

OVMSDK_API STATUS StartEnrollMode(OVSGetPreviewCallback previewCB,
                       OVSGetFeatureCallback featureCB);
OVMSDK_API STATUS StartMatchMode(OVSGetPreviewCallback previewCB,
                      OVSGetFeatureCallback featureCB);

OVMSDK_API STATUS GetPreviewImage(OVS::Image& image,string &feature,OVS::Information& info);


OVMSDK_API STATUS AbortCapture();

}
#endif

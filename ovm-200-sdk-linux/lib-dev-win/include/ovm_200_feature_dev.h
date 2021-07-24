#ifndef OVM_200_FEATURE_DEV_H
#define OVM_200_FEATURE_DEV_H

#include <string>

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


namespace OVS_DEV
{
    class FeatureDevelopTool
    {
    public:
        OVMSDK_API __stdcall FeatureDevelopTool();
        OVMSDK_API __stdcall ~FeatureDevelopTool();

        OVMSDK_API int __stdcall getFeatureMax();
        OVMSDK_API void __stdcall setFeatureMax(const int max);

        OVMSDK_API int  __stdcall getDistanceSensorLow();
        OVMSDK_API void __stdcall setDistanceSensorLow(const int low);

        OVMSDK_API int __stdcall getDistanceSensorHigh();
        OVMSDK_API void __stdcall setDistanceSensorHigh(const int hi);

        //flash
        OVMSDK_API bool __stdcall readFromFlash();
        OVMSDK_API bool __stdcall write2Flash();

        OVMSDK_API bool __stdcall setDevName(const string& strName);
        OVMSDK_API bool __stdcall getDevName(string& strName);

        OVMSDK_API bool __stdcall setDevSN(const string& strSN);
        OVMSDK_API bool __stdcall getDevSN(string& strSN);

        OVMSDK_API bool __stdcall setDevFireWareVer(const string& s);
        OVMSDK_API bool __stdcall getDevFireWareVer(string& s);

        OVMSDK_API bool __stdcall setDevHardWareVer(const string& strHWVer);
        OVMSDK_API bool __stdcall getDevHardWareVer(string& strHWVer);

        OVMSDK_API bool __stdcall setDevSoftWareVer(const string& strSWVer);
        OVMSDK_API bool __stdcall getDevSoftWareVer(string& strSWVer);
    };
    enum DevReturnValue 
    {
        ERR_SUCC,
    };
}
#endif

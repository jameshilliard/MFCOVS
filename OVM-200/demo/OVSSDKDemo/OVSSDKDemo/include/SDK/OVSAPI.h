/**
*  Sqlite control Functions Declaration
*
*  @author		Xu huabin, Ji renze
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			OVS Sqlite Control API Declaration
*
**/

/**
*
*  Copyright (c) 2020 Qingdao NovelBeam Technology Co., Ltd. All Rights Reserved.
*
*  This software is the confidential and proprietary information of Qingdao NovelBeam Technology Co., Ltd. ("Confidential Information").
*  You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement
*  you entered into with Qingdao NovelBeam Technology Co., Ltd.
*
**/

#ifndef __OVS_API_H__
#define __OVS_API_H__

#ifdef WIN32
#if defined(__AFXSTR_H__) || defined(__ATLSTR_H__)
#include <atlstr.h>
#endif //defined(__AFXSTR_H__) || defined(__ATLSTR_H__)
#ifdef OVSSDK_EXPORTS
#define OVSSDK_API __declspec(dllexport) __stdcall
#else //OVSSDK_EXPORTS
#define OVSSDK_API __declspec(dllimport) __stdcall
#endif //OVSSDK_EXPORTS
#else //WIN32
#define OVSSDK_API
#define __stdcall
typedef int BOOL;
#endif //WIN32

#include <string>
#include <memory>
#include "OVSStatus.h"

using namespace std;
//typedef void(__stdcall* OVSFeatureCallback)(int reulst, char* feature, int feature_len);
//typedef void(__stdcall* OVSPreviewCallback)(int reulst, int stage_progress, int palm_stability, unsigned char* image_data, int image_width, int image_height);

/**
* @brief		Initialize SDK function
* @details		The OVS_SDK is initialized and can use SDK APIs
*
* @param[in]	sdk_mode	: for sdk mode (only matcher or both)
*
* @return		Refer to OVSErrorCode enum
*

*/
OVSSTATUS OVSSDK_API OVS_InitSDK(void);

// 曝光
OVSSTATUS OVSSDK_API OVS_Set_Exposure(const int val);
OVSSTATUS OVSSDK_API OVS_Get_Exposure(int& val);

// LED
OVSSTATUS OVSSDK_API OVS_Set_LEDCurrent(const int val);
OVSSTATUS OVSSDK_API OVS_Get_LEDCurrent(int& val);

// BL
OVSSTATUS OVSSDK_API OVS_Set_BLOpen(const bool flag);
OVSSTATUS OVSSDK_API OVS_Get_BL(int& val);

// 增益
OVSSTATUS OVSSDK_API OVS_Set_Gain(int val);
OVSSTATUS OVSSDK_API OVS_Get_Gain(int& val);

// 图像压缩率
OVSSTATUS OVSSDK_API OVS_Set_CompressionRatio(int val);
OVSSTATUS OVSSDK_API OVS_Get_CompressionRatio(int& val);

// 距离
OVSSTATUS OVSSDK_API OVS_Get_Distance(int& val);

// 显示
OVSSTATUS OVSSDK_API OVS_GetPreviewFullImage(Image& in_image);
/**
* @brief		Un-initialize SDK function
* @details		The OVS_SDK is a un-initialized and cannot use SDK APIs.
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_UninitSDK(void);

/**
* @brief		Get Feature For Enroll function
* @details		Get Feature For Enroll
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_StartFeatureForEnroll();

/**
* @brief		Get Feature For Enroll function
* @details		Get Feature For Enroll
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_StartFeatureForMatch();

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_GetPreviewImage(Image& image, Information& info);
//
///**
//* @brief		Get Preview Image function
//* @details		Get Preview Image from device
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVSSTATUS OVSSDK_API OVS_GetPreviewImage(Image& image, string& feature, Information& info);


//typedef void(stdcall* OVSPreviewCallback)(OVS::Image& image, OVS::Information& info);
//
//
////启用此接口 不用调用提取特征点接口函数获取特征数据；
//typedef void(stdcall* OVSPreviewCallback)(OVS::Image& image, std::string& feature, OVS::Information& info);
//
///**
//* @brief		Get Preview Image function
//* @details		Get Preview Image from device
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVSSTATUS OVSSDK_API OVS_GetFeatureForEnroll(OVSPreviewCallback* call);
//
///**
//* @brief		Get Preview Image function
//* @details		Get Preview Image from device
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVSSTATUS OVSSDK_API OVS_GetFeatureForMatch(OVSPreviewCallback* call);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_AbortCapture();

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_GetEnrollFeature(char* feature);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_GetMatchFeature(char* feature);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_MatchVerify(const std::string featureSrc, const std::string featureDest);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_MatchIdentify(const std::string feature, std::string& userID);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_UserCount();

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_AddUser(char* userID, char* featureLeft, char* featureRight);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_UpdateUser(const std::string userID, const std::string featureLeft, const std::string featureRight);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_DeleteUser(const std::string userID);

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_DeleteAllUsers();

/**
* @brief		Get Preview Image function
* @details		Get Preview Image from device
*
* @return		Refer to OVSErrorCode enum
*
*/
OVSSTATUS OVSSDK_API OVS_GetErrorString(const int& errorCode);

#endif //__OVS_API_H__
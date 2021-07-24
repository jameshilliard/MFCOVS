/**
*  Device control Functions Declaration
*
*  @author		Xu huabin, Ji renze
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Device Control API Declaration
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

//#include "ovs_data.h"
//#include "ovs_error.h"

//#define OVS_STATUS int
//
//
//typedef void(__stdcall* OVSFeatureCallback)(int reulst, char* feature, int feature_len);
//typedef void(__stdcall* OVSPreviewCallback)(int reulst, int stage_progress, int palm_stability, unsigned char* image_data, int image_width, int image_height);
//
//
///**
//* @brief		Initialize SDK function
//* @details		The OVS_SDK is initialized and can use SDK APIs
//*
//* @param[in]	sdk_mode	: for sdk mode (only matcher or both)
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_InitSDK(void);
//
//
///**
//* @brief		Un-initialize SDK function
//* @details		The OVS_SDK is a un-initialized and cannot use SDK APIs.
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_UninitSDK(void);
//
//
///**
//* @brief		Abort capture function
//* @details		Abort capturing for authentication and enrollment
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_AbortCapture(void);
//
//
///**
//* @brief		Save bitmap function
//* @details		To save a preview image of byte data
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_SaveBitmap(unsigned char* image_data, int image_width, int image_height, const char* image_filename);
//
//
///**
//* @brief		Register enrollment callback
//* @details		Register to get preview image when capturing for enrollment and to get palm-vein feature when finishing enrollment
//*
//* @param[in]	preview_callback	: callback to get preview image
//* @param[in]	feature_callback	: callback to get feature
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_RegisterEnrollCallback(OVSPreviewCallback preview_callback, OVSFeatureCallback feature_callback);
//
//
///**
//* @brief		Register matching callback
//* @details		Register to get preview image when capturing for matching and to get palm-vein feature when finishing matching
//*
//* @param[in]	preview_callback	: callback to get preview image
//* @param[in]	feature_callback	: callback to get feature
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_RegisterMatchCallback(OVSPreviewCallback preview_callback, OVSFeatureCallback feature_callback);
//
//
///**
//* @brief		Get enrollment feature
//* @details		Process to get a feature when finishing enrollment
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_GetFeatureForEnroll(void);
//
//
///**
//* @brief		Get feature for matching
//* @details		Process to get a feature when finishing matching
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_GetFeatureForMatch(void);
//
//
///**
//* @brief		Verification function
//* @details		To get 1:1 matching result
//*
//* @param[in]	user_key	: user's key to identify
//* @param[in]	user_fea	: user's feature to identify
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_MatchVerify(const char* user_key, const char* user_fea);
//
//
///**
//* @brief		Identification function
//* @details		To get 1:N matching result
//*
//* @param[in]	user_fea	: user's feature to identify
//* @param[in]	timeout		: timeout of matching (ms)
//* @param[out]	matched_key	: user's key from matching result
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_MatchIdentify(const char* user_fea, int timeout, char* matched_key);
//
//
///**
//* @brief		User count function
//* @details		To get the number of users in memory
//*
//* @return		The number of users
//*
//*/
//OVS_STATUS OVSSDK_API OVS_UserCount(void);
//
//
///**
//* @brief		Add user function
//* @details		To add id and features of new user in memory
//*
//* @param[in]	user_key	: user's key to add
//* @param[in]	left_fea	: user's left feature
//* @param[in]	right_fea	: user's right feature
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_AddUser(const char* user_key, const char* left_fea, const char* right_fea);
//
//
///**
//* @brief		Update user function
//* @details		To update features of exist user in memory
//*
//* @param[in]	user_key	: user's key to update
//* @param[in]	left_fea	: user's left feature
//* @param[in]	right_fea	: user's right feature
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_UpdateUser(const char* user_key, const char* left_fea, const char* right_fea);
//
//
///**
//* @brief		Delete user function
//* @details		To delete information of exist user in memory
//*
//* @param[in]	user_key	: user's key to delete
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_DeleteUser(const char* user_key);
//
//
///**
//* @brief		Delete all users
//* @details		To delete all users in memory
//*
//* @return		Refer to OVSErrorCode enum
//*
//*/
//OVS_STATUS OVSSDK_API OVS_DeleteAllUsers(void);
//
//
///**
//* @brief		Get error string
//* @details		To get an error-string of OVSErrorCode enum
//*
//* @param[in]	error_code	: error code value
//* @param[in]	error_msg	: error message
//*
//* @return		OVS_SUCCESS
//*
//*/
//OVS_STATUS OVSSDK_API OVS_GetErrorString(int error_code, char* error_msg);
//
//
///**
//* @brief		Get status string
//* @details		To get status-string of OVSCaptureStatus enum
//*
//* @param[in]	status_code	: status code value
//* @param[in]	status_msg	: status message
//*
//* @return		OVS_SUCCESS
//*
//*/
//OVS_STATUS OVSSDK_API OVS_GetStatusString(int status_code, char* status_msg);



#endif __OVS_API_H__

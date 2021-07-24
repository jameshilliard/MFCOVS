/**
*  Inomec OVS SDK Error Code
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao O-Mec Biometrics Co., Ltd.
*  @see			Error Code
*
**/

/**
*
*  Copyright (c) 2018 Qingdao O-Mec Biometrics Co., Ltd. All Rights Reserved.
*
*  This software is the confidential and proprietary information of Qingdao O-Mec Biometrics Co., Ltd. ("Confidential Information").
*  You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement
*  you entered into with Qingdao O-Mec Biometrics Co., Ltd.
*
**/

#ifndef __OVS_ERROR_H__
#define __OVS_ERROR_H__

typedef enum 
{
	OVS_SUCCESS = 0,
	OVS_ERROR = -1,
	
	OVS_ERR_SDK_UNINITIALIZED = -10,
	OVS_ERR_SDK_ALREADY_INITIALIZED = -11,
	OVS_ERR_COMPORT_ALREADY_USED = -12,

	OVS_ERR_NO_DEVICE = -100,
	OVS_ERR_INVALID_LICENSE = -101,
	OVS_ERR_INVALID_DEVICE_INDEX = -102,
	OVS_ERR_NOT_YET_SUPPORTED = -103,
	OVS_ERR_INVALID_PARAM = -104,
	OVS_ERR_NOT_REGISTERED_CALLBACK = -105,

	OVS_ERR_CAPTURE_DIABLED = -200,
	OVS_ERR_CAPTURE_ABORTED = -201,
	OVS_ERR_CAPTURE_TIMEOUT = -202,
	OVS_ERR_CAPTURE_IS_RUNNING = -203,
	OVS_ERR_CAPTURE_IS_NOT_RUNNING = -204,

	OVS_ERR_ALREADY_EXIST_USER = -300,
	OVS_ERR_FAILED_TO_ADD_USER = -301,
	OVS_ERR_FAILED_TO_UPDATE_USER = -302,
	OVS_ERR_FAILED_TO_DELETE_USER = -303,
	OVS_ERR_NO_EXIST_USER = -304,

	OVS_ERR_MATCHING_TIMEOUT = -400,
	OVS_ERR_MATCHING_FAILED = -401,
	

} OVSErrorCode;

#endif //__OVS_ERROR_H__
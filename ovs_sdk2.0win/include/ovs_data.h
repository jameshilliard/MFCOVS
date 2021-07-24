/**
*  Inomec OVS SDK Data Structure
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao O-Mec Biometrics Co., Ltd.
*  @see			Data Structure Definitions
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

#ifndef __OVS_DATA_H__
#define __OVS_DATA_H__

typedef enum
{
	OVS_GET_FEATURE_SUCCEED = 10,
	OVS_GET_FEATURE_FAILED = 11,
	OVS_GET_FEATURE_PALM_OUT = 12,

	OVS_GET_PREVIEW_UNKNOWN = 100,
	OVS_GET_PREVIEW_FAILED = 101,
	OVS_GET_PREVIEW_SUCCEED = 102,
	OVS_GET_PREVIEW_1ST_PROCESSING = 103,
	OVS_GET_PREVIEW_2ND_PROCESSING = 104,
	OVS_GET_PREVIEW_3RD_PROCESSING = 105,
	OVS_GET_PREVIEW_PALM_OUT = 106,
	OVS_GET_PREVIEW_REMOVE_PALM_ON_THE_SENSOR = 107,
	OVS_GET_PREVIEW_PUT_PALM_ON_THE_SENSOR = 108,

	OVS_GET_PALM_STABLE_BAD = 200,
	OVS_GET_PALM_STABLE_NORMAL = 201,
	OVS_GET_PALM_STABLE_GOOD = 202,

} OVSCaptureStatus;

typedef enum
{
	OVS_SDK_BOTH = 0,
	OVS_ONLY_MATCHER = 1,
} OVSSdkMode;

#endif //__OVS_DATA_H__
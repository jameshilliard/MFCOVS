/**
*  Device Functions Declaration
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Standard API Declaration
*
**/

/**
*
*  Copyright (c) 2018 Qingdao NovelBeam Technology Co., Ltd. All Rights Reserved.
*
*  This software is the confidential and proprietary information of Qingdao NovelBeam Technology Co., Ltd. ("Confidential Information").
*  You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement
*  you entered into with Qingdao NovelBeam Technology Co., Ltd.
*
**/

#include "whois_feature_c.h"
#include "whois_match.h"

#include "ovs_internal.h"
#include "ovs_api.h"

COvsSerial* g_ovs_serial = NULL;
COvsTimer* g_ovs_timer = NULL;
COvsInit* g_ovs_init = NULL;
COvsUserInfos* g_ovs_userinfos = NULL;
ovs_ini::OVS_CFG_t g_ovs_cfg;

OVS_STATUS OVSSDK_API OVS_InitSDK(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API called\n");
	
	g_ovs_serial = new COvsSerial();
	g_ovs_timer = new COvsTimer(false);
	g_ovs_init = new COvsInit();
	g_ovs_userinfos = new COvsUserInfos();

	if (ovs_ini::loadIniFile() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API - loadIniFile is failed\n");
	}
	if (!ovs_extlib::isExist()) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API - external library is not exist\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (g_ovs_init->isAliveDevice() == true) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API - already initialized\n");
		return OVS_ERR_SDK_ALREADY_INITIALIZED;
	}
	if (g_ovs_serial->isOpenSerial() == true) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API - already used comport\n");
		return OVS_ERR_COMPORT_ALREADY_USED;
	}
	g_ovs_cfg = ovs_ini::getConfig();
	clear_partitions();
	set_partition_options(0);
	g_ovs_userinfos->deleteAll();
	if (open_device_c(s_device_id) != OVS_SUCCESS) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API - Not initialized\n");
		return OVS_ERR_INVALID_DEVICE_INDEX;
	}
	if (g_ovs_serial->openSerial() == OVS_ERROR) {
		if (!ovs_license::isValid()) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API - Invalid license");
			return OVS_ERR_INVALID_LICENSE;
		}
	}
	g_ovs_serial->ledControl("S1");
	g_ovs_init->openDevice();
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_InitSDK API finished\n");
	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_UninitSDK(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UninitSDK API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UninitSDK API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	
	g_ovs_serial->ledControl("S1");
	g_ovs_init->Reset();
	clear_partitions();
	g_ovs_userinfos->deleteAll();

	if (s_enrollimage_data != NULL) {
		delete[] s_enrollimage_data;
		s_enrollimage_data = NULL;
	}
	if (s_matchimage_data != NULL) {
		delete[] s_matchimage_data;
		s_matchimage_data = NULL;
	}

	if (close_device_c(s_device_id) != OVS_SUCCESS) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UninitSDK API - Not uninitialized\n");
		return OVS_ERR_INVALID_DEVICE_INDEX;
	}

	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UninitSDK API finished\n");
	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_SaveBitmap(unsigned char* image_data, int image_width, int image_height, const char* image_filename)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_SaveBitmap API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_SaveBitmap API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}

	if (image_width != 640 || image_height != 480) return OVS_ERR_INVALID_PARAM;

	if (!image_data) return OVS_ERR_INVALID_PARAM;

	int targetWidth = (image_width % 4 == 0) ? image_width : image_width + 4 - image_width % 4;

	BITMAPFILEHEADER fileHeader;

	BITMAPINFO* bitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
	if (!bitmapInfo)
	{
		return OVS_ERROR;
	}

	bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo->bmiHeader.biWidth = image_width;
	bitmapInfo->bmiHeader.biHeight = image_height;
	bitmapInfo->bmiHeader.biPlanes = 1;
	bitmapInfo->bmiHeader.biBitCount = 8;
	bitmapInfo->bmiHeader.biCompression = BI_RGB;
	bitmapInfo->bmiHeader.biSizeImage = 0;
	bitmapInfo->bmiHeader.biXPelsPerMeter = 19686; // 500 * 39.37
	bitmapInfo->bmiHeader.biYPelsPerMeter = 19686;
	bitmapInfo->bmiHeader.biClrUsed = 0;
	bitmapInfo->bmiHeader.biClrImportant = 0;

	for (int i = 0; i < 256; i++)
	{
		bitmapInfo->bmiColors[i].rgbRed = i;
		bitmapInfo->bmiColors[i].rgbGreen = i;
		bitmapInfo->bmiColors[i].rgbBlue = i;
	}

	FILE* fp;
	fopen_s(&fp, image_filename, "wb");

	if (!fp)
	{
		free(bitmapInfo);
		return OVS_ERROR;
	}

	// Fill in the fields of the file header 
	fileHeader.bfType = ((WORD)('M' << 8) | 'B');	// is always "BM"
	fileHeader.bfSize = sizeof(fileHeader) + sizeof(bitmapInfo) + targetWidth * image_height;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = (DWORD)(sizeof(fileHeader) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

	int size = sizeof(fileHeader);

	if (fwrite(&fileHeader, 1, size, fp) != size)
	{
		fclose(fp);
		free(bitmapInfo);

		return OVS_ERROR;
	}

	size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	if (fwrite(bitmapInfo, 1, size, fp) != size)
	{
		fclose(fp);
		free(bitmapInfo);

		return OVS_ERROR;
	}

	size = targetWidth * image_height;

	unsigned char* targetBuf = (unsigned char*)malloc(size);

	if (!targetBuf)
	{
		fclose(fp);
		free(bitmapInfo);

		return OVS_ERROR;
	}

	memset(targetBuf, 0, size);

	for (int i = 0; i < image_height; i++)
	{
		memcpy(targetBuf + (image_height - 1 - i) * targetWidth,
			image_data + i * image_width, image_width);
	}

	if (fwrite(targetBuf, 1, size, fp) != size)
	{
		fclose(fp);
		free(bitmapInfo);
		free(targetBuf);

		return OVS_ERROR;
	}

	fclose(fp);
	free(bitmapInfo);
	free(targetBuf);

	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_SaveBitmap API finished\n");
	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_GetErrorString(int error_code, char* error_msg)
{
	error_msg[0] = '\0';
	switch (error_code)
	{
	case OVS_SUCCESS:
		strcpy(error_msg, "The function is succeed");
		break;
	case OVS_ERROR:
		strcpy(error_msg, "The function is failed");
		break;
	case OVS_ERR_SDK_UNINITIALIZED:
		strcpy(error_msg, "The SDK is not initialized.");
		break;
	case OVS_ERR_SDK_ALREADY_INITIALIZED:
		strcpy(error_msg, "The SDK is already initialized.");
		break;
	case OVS_ERR_NO_DEVICE:
		strcpy(error_msg, "The device is unplugged.");
		break;
	case OVS_ERR_INVALID_LICENSE:
		strcpy(error_msg, "The license is invalid.");
		break;
	case OVS_ERR_INVALID_DEVICE_INDEX:
		strcpy(error_msg, "The device index is invalid.");
		break;
	case OVS_ERR_NOT_YET_SUPPORTED:
		strcpy(error_msg, "The function is not yet supported.");
		break;
	case OVS_ERR_INVALID_PARAM:
		strcpy(error_msg, "One of the parameters is invalid.");
		break;
	case OVS_ERR_NOT_REGISTERED_CALLBACK:
		strcpy(error_msg, "The registered callback is null.");
		break;
	case OVS_ERR_CAPTURE_DIABLED:
		strcpy(error_msg, "The capture is disabled.");
		break;
	case OVS_ERR_CAPTURE_ABORTED:
		strcpy(error_msg, "The capture is aborted.");
		break;
	case OVS_ERR_CAPTURE_TIMEOUT:
		strcpy(error_msg, "The capture is timeout.");
		break;
	case OVS_ERR_CAPTURE_IS_RUNNING:
		strcpy(error_msg, "The capture is running.");
		break;
	case OVS_ERR_CAPTURE_IS_NOT_RUNNING:
		strcpy(error_msg, "The capture is not running.");
		break;
	case OVS_ERR_ALREADY_EXIST_USER:
		strcpy(error_msg, "The user is already exist.");
		break;
	case OVS_ERR_FAILED_TO_ADD_USER:
		strcpy(error_msg, "To add the user is failed.");
		break;
	case OVS_ERR_FAILED_TO_UPDATE_USER:
		strcpy(error_msg, "To update the user is failed.");
		break;
	case OVS_ERR_FAILED_TO_DELETE_USER:
		strcpy(error_msg, "To delete the user is failed.");
		break;
	case OVS_ERR_MATCHING_TIMEOUT:
		strcpy(error_msg, "The matching is timeout.");
		break;
	case OVS_ERR_MATCHING_FAILED:
		strcpy(error_msg, "The matching is failed.");
		break;
	default:
		sprintf(error_msg, "Unknown error: %d", error_code);
		break;
	}

	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_GetStatusString(int status_code, char* status_msg)
{
	status_msg[0] = '\0';
	switch (status_code)
	{
	case OVS_GET_FEATURE_SUCCEED:
		strcpy(status_msg, "The routine of a getting feature is succeed.");
		break;
	case OVS_GET_FEATURE_FAILED:
		strcpy(status_msg, "The routine of a getting feature is failed.");
		break;
	case OVS_GET_PREVIEW_UNKNOWN:
		strcpy(status_msg, "The stage of a getting preview is unknown error.");
		break;
	case OVS_GET_PREVIEW_FAILED:
		strcpy(status_msg, "The stage of a getting preview is failed.");
		break;
	case OVS_GET_PREVIEW_SUCCEED:
		strcpy(status_msg, "The stage of a getting preview is succeed.");
		break;
	case OVS_GET_PREVIEW_1ST_PROCESSING:
		strcpy(status_msg, "The processing is the first stage.");
		break;
	case OVS_GET_PREVIEW_2ND_PROCESSING:
		strcpy(status_msg, "The processing is the second stage.");
		break;
	case OVS_GET_PREVIEW_3RD_PROCESSING:
		strcpy(status_msg, "The processing is the third stage.");
		break;
	case OVS_GET_FEATURE_PALM_OUT:
	case OVS_GET_PREVIEW_PALM_OUT:
		strcpy(status_msg, "The palm is out on the sensor");
		break;
	case OVS_GET_PREVIEW_REMOVE_PALM_ON_THE_SENSOR:
		strcpy(status_msg, "Remove the palm on the sensor.");
		break;
	case OVS_GET_PREVIEW_PUT_PALM_ON_THE_SENSOR:
		strcpy(status_msg, "Put the palm on the sensor.");
		break;
	default:
		sprintf(status_msg, "Unknown status: %d", status_code);
		break;
	}

	return OVS_SUCCESS;
}

/*
OVS_STATUS OVSSDK_API OVS_GetParam(int param_mode, void* value1)
{
	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_SetParam(int param_mode, void* value1)
{
	return OVS_SUCCESS;
}
*/
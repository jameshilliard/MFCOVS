/**
*  Feature Functions Declaration
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

#include "ovs_internal.h"
#include "ovs_api.h"


static OVSPreviewCallback s_enrollpreview_callback = NULL;
static OVSPreviewCallback s_matchpreview_callback = NULL;
static OVSFeatureCallback s_enrollfeature_callback = NULL;
static OVSFeatureCallback s_matchfeature_callback = NULL;

bool __stdcall enrollcallback_init(int image_width, int image_height)
{
	s_enrollimage_size = image_width * image_height;
	if (s_enrollimage_data != NULL) {
		if (s_enrollimage_width != image_width || s_enrollimage_height != image_height) {
			delete[] s_enrollimage_data;
			s_enrollimage_data = new unsigned char[s_enrollimage_size];
		}
	}
	else {
		s_enrollimage_data = new unsigned char[s_enrollimage_size];
	}
	s_enrollimage_width = image_width;
	s_enrollimage_height = image_height;
	return true;
}

void __stdcall enrollcallback_attach(unsigned char* image_data, int pos_cx, int pos_cy)
{
	int _stage_value = s_enrollstage_value;
	if (s_enrollimage_data == NULL) {
		s_enrollimage_data = new unsigned char[s_enrollimage_size];
	}

	if (image_data == NULL) {
		memset(s_enrollimage_data, 0x00, s_enrollimage_size);
		_stage_value = OVS_GET_PREVIEW_FAILED;
	}
	else {
		memcpy(s_enrollimage_data, image_data, s_enrollimage_size);
		ovs_image::flipV(s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
		_stage_value = s_enrollstage_value;
	}

	if (s_enrollpreview_callback) {
		int palm_stability = is_stable_palm(pos_cx, pos_cy);
		s_enrollpreview_callback(_stage_value, s_enrollstage_candget, palm_stability, s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
	}
}

void __stdcall enrollcallback_notify(unsigned int msg, unsigned int param1, unsigned int param2)
{
	if (s_enrollimage_size <= 0) return;

	switch (msg)
	{
	case OVSIN_ENROLL_STAGE:
		{
			switch (param2)
			{
			case 0:
				if (s_enrollpreview_callback) {
					s_enrollpreview_callback(OVS_GET_PREVIEW_PUT_PALM_ON_THE_SENSOR, s_enrollstage_candget, OVS_GET_PALM_STABLE_BAD,
						s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
				}
				break;
			case 1:
				s_enrollstage_value = OVS_GET_PREVIEW_1ST_PROCESSING;
				break;
			case 2:
				if (s_enrollpreview_callback) {
					s_enrollpreview_callback(OVS_GET_PREVIEW_REMOVE_PALM_ON_THE_SENSOR, s_enrollstage_candget, OVS_GET_PALM_STABLE_BAD,
						s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
				}
				g_ovs_serial->ledControl("P2");
				break;

			case 3:
				if (s_enrollpreview_callback) {
					s_enrollpreview_callback(OVS_GET_PREVIEW_PUT_PALM_ON_THE_SENSOR, s_enrollstage_candget, OVS_GET_PALM_STABLE_BAD,
						s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
				}
				break;
			case 4:
				s_enrollstage_value = OVS_GET_PREVIEW_2ND_PROCESSING;
				break;
			case 5:
				if (s_enrollpreview_callback) {
					s_enrollpreview_callback(OVS_GET_PREVIEW_REMOVE_PALM_ON_THE_SENSOR, s_enrollstage_candget, OVS_GET_PALM_STABLE_BAD,
						s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
				}
				g_ovs_serial->ledControl("P3");
				break;

			case 6:
				if (s_enrollpreview_callback) {
					s_enrollpreview_callback(OVS_GET_PREVIEW_PUT_PALM_ON_THE_SENSOR, s_enrollstage_candget, OVS_GET_PALM_STABLE_BAD,
						s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
				}
				break;
			case 7:
				s_enrollstage_value = OVS_GET_PREVIEW_3RD_PROCESSING;
				break;
			default:
				s_enrollstage_value = OVS_GET_PREVIEW_UNKNOWN;
				break;
			}
		}
		break;

	case OVSIN_ENROLL_CANDIDATE:
		s_enrollstage_candget = param1;
		break;

	case OVSIN_ENROLL_SUCCEED:
		{
			g_ovs_serial->ledControl("S1");
			int enroll_feature_len = 64000;
			char* enroll_feature = new char[enroll_feature_len];
			memset(enroll_feature, 0, enroll_feature_len);
			if (get_enroll_feature_c(enroll_feature, enroll_feature_len, s_device_id) == OVS_SUCCESS) {
				if (s_enrollfeature_callback) s_enrollfeature_callback(OVS_GET_FEATURE_SUCCEED, enroll_feature, enroll_feature_len);
			}
			if (enroll_feature != NULL) {
				delete[] enroll_feature;
				enroll_feature = NULL;
			}
			g_ovs_init->stopEnroll();
		}
		break;

	case OVSIN_ENROLL_FAIL:
		g_ovs_serial->ledControl("R1");
		if (s_enrollfeature_callback) s_enrollfeature_callback(OVS_GET_FEATURE_FAILED, NULL, 0);
		g_ovs_init->stopEnroll();
		break;

	case OVSIN_ENROLL_PALM_OUT:
		if (s_enrollpreview_callback) {
			s_enrollpreview_callback(OVS_GET_PREVIEW_PALM_OUT, s_enrollstage_candget, OVS_GET_PALM_STABLE_BAD,
				s_enrollimage_data, s_enrollimage_width, s_enrollimage_height);
		}
		break;
	}
}

bool __stdcall featurecallback_init(int image_width, int image_height)
{
	s_matchimage_size = image_width * image_height;
	if (s_matchimage_data != NULL) {
		if (s_matchimage_width != image_width || s_matchimage_height != image_height) {
			delete[] s_matchimage_data;
			s_matchimage_data = new unsigned char[s_matchimage_size];
		}
	}
	else {
		s_matchimage_data = new unsigned char[s_matchimage_size];
	}
	s_matchimage_width = image_width;
	s_matchimage_height = image_height;
	return true;
}

void __stdcall featurecallback_attach(unsigned char* image_data, int pos_cx, int pos_cy)
{
	int _match_res = OVS_GET_PREVIEW_1ST_PROCESSING;
	if (s_matchimage_data == NULL) {
		s_matchimage_data = new unsigned char[s_matchimage_size];
	}

	if (image_data == NULL) {
		_match_res = OVS_GET_PREVIEW_FAILED;
		memset(s_matchimage_data, 0x00, s_matchimage_size);
	}
	else {
		_match_res = OVS_GET_PREVIEW_1ST_PROCESSING;
		memcpy(s_matchimage_data, image_data, s_matchimage_size);
		ovs_image::flipV(s_matchimage_data, s_matchimage_width, s_matchimage_height);
	}

	if (!g_ovs_init->isCaptureDone())
	{
		if (s_matchpreview_callback) {
			int palm_stability = is_stable_palm(pos_cx, pos_cy);
			s_matchpreview_callback(_match_res, 0, palm_stability, s_matchimage_data, s_matchimage_width, s_matchimage_height);
		}
	}
}

void __stdcall featurecallback_notify(unsigned int msg, unsigned int param1, unsigned int param2)
{
	switch (msg)
	{
	case OVSIN_FEATURE_PALM_IN:
		break;

	case OVSIN_FEATURE_PALM_OUT:
		g_ovs_timer->startTimer();
		g_ovs_init->captureResume();
		g_ovs_serial->ledControl("Y1");
		if (s_matchfeature_callback) s_matchfeature_callback(OVS_GET_FEATURE_PALM_OUT, NULL, 0);
		break;

	case OVSIN_FEATURE_GET_OK:
		if (!g_ovs_init->isCaptureDone())
		{
			int match_feature_len = 64000;
			char* match_feature = new char[match_feature_len];
			memset(match_feature, 0, match_feature_len);
			if (get_feature_c(match_feature, match_feature_len, s_device_id) == OVS_SUCCESS) {
				if (s_matchfeature_callback) s_matchfeature_callback(OVS_GET_FEATURE_SUCCEED, match_feature, match_feature_len);
			}
			if (match_feature != NULL) {
				delete[] match_feature;
				match_feature = NULL;
			}
			g_ovs_init->stopFeature();
		}
		break;

	case OVSIN_FEATURE_GET_ERROR:
		if (!g_ovs_init->isCaptureDone())
		{
			if (s_matchfeature_callback) s_matchfeature_callback(OVS_GET_FEATURE_FAILED, NULL, 0);
			g_ovs_init->stopFeature();
		}
		break;
	}
}

OVS_STATUS OVSSDK_API OVS_RegisterEnrollCallback(OVSPreviewCallback preview_callback, OVSFeatureCallback feature_callback)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterEnrollCallback API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterEnrollCallback API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (g_ovs_init->isStatusEnroll() == true || g_ovs_init->isStatusFeature() == true) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterEnrollCallback API - Is capturing (enroll:%d, feature:%d)\n", g_ovs_init->isStatusEnroll(), g_ovs_init->isStatusFeature());
		return OVS_ERR_CAPTURE_IS_RUNNING;
	}
	s_enrollpreview_callback = preview_callback;
	s_enrollfeature_callback = feature_callback;
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterEnrollCallback API finished\n");
	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_RegisterMatchCallback(OVSPreviewCallback preview_callback, OVSFeatureCallback feature_callback)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterMatchCallback API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterMatchCallback API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (g_ovs_init->isStatusEnroll() == true || g_ovs_init->isStatusFeature() == true) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterMatchCallback API - Is capturing (enroll:%d, feature:%d)\n", g_ovs_init->isStatusEnroll(), g_ovs_init->isStatusFeature());
		return OVS_ERR_CAPTURE_IS_RUNNING;
	}
	s_matchpreview_callback = preview_callback;
	s_matchfeature_callback = feature_callback;
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_RegisterMatchCallback API finished\n");
	return OVS_SUCCESS;
}

OVS_STATUS OVSSDK_API OVS_GetFeatureForEnroll(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForEnroll API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForEnroll API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (g_ovs_init->isStatusEnroll() == true || g_ovs_init->isStatusFeature() == true) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForEnroll API - Is capturing (enroll:%d, feature:%d)\n", g_ovs_init->isStatusEnroll(), g_ovs_init->isStatusFeature());
		return OVS_ERR_CAPTURE_IS_RUNNING;
	}
	if (s_enrollpreview_callback == NULL) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForEnroll API - preview callback is null\n");
		return OVS_ERR_NOT_REGISTERED_CALLBACK;
	}
	else {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForEnroll API - preview callback (address:0x%x)\n", s_enrollpreview_callback);
	}
	int ovs_res = OVS_SUCCESS;
	if (g_ovs_init->isAliveEnroll() == false) {
		ovs_res = g_ovs_init->createEnroll(s_device_id);
	}
	if ((ovs_res | create_enroll_options(s_device_id)) == OVS_SUCCESS) {
		if (start_enroll_c(&enrollcallback_init, &enrollcallback_attach, &enrollcallback_notify, s_device_id) != OVS_SUCCESS) {
			g_ovs_serial->ledControl("S1");
			ovs_res = OVS_ERR_CAPTURE_DIABLED;
		}
		else {
			g_ovs_serial->ledControl("P1");
			g_ovs_init->startEnroll();
			ovs_res = OVS_SUCCESS;
		}
	}
	else {
		ovs_res = OVS_ERR_CAPTURE_DIABLED;
	}
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForEnroll API finished (res:%d)\n", ovs_res);
	return ovs_res;
}

OVS_STATUS OVSSDK_API OVS_GetFeatureForMatch(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForMatch API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForMatch API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (g_ovs_init->isStatusEnroll() == true || g_ovs_init->isStatusFeature() == true) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForMatch API - Is capturing (enroll:%d, feature:%d)\n", g_ovs_init->isStatusEnroll(), g_ovs_init->isStatusFeature());
		return OVS_ERR_CAPTURE_IS_RUNNING;
	}
	if (s_matchpreview_callback == NULL) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForMatch API - preview callback is null\n");
		return OVS_ERR_NOT_REGISTERED_CALLBACK;
	}
	else {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForMatch API - preview callback (address:0x%x)\n", s_matchpreview_callback);
	}
	int ovs_res = OVS_SUCCESS;
	if (g_ovs_init->isAliveFeature() == false) {
		ovs_res = g_ovs_init->createFeature(s_device_id);
	}
	if ((ovs_res|create_match_options(s_device_id)) == OVS_SUCCESS) {
		if (start_feature_c(&featurecallback_init, &featurecallback_attach, &featurecallback_notify, s_device_id) != OVS_SUCCESS) {
			g_ovs_serial->ledControl("S1");
			ovs_res = OVS_ERR_CAPTURE_DIABLED;
		}
		else {
			g_ovs_serial->ledControl("Y1");
			g_ovs_init->startFeature();
		}
	}
	else {
		ovs_res = OVS_ERR_CAPTURE_DIABLED;
	}
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_GetFeatureForMatch API finished (res:%d)\n", ovs_res);
	return ovs_res;
}

OVS_STATUS OVSSDK_API OVS_AbortCapture(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API called\n");
	int ovs_res = OVS_SUCCESS;

	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}

	if (g_ovs_init->isAliveEnroll() == true) {
		if (stop_enroll_c(s_device_id) != OVS_SUCCESS) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API - Enroll stop is failed\n");
			return OVS_ERR_CAPTURE_IS_NOT_RUNNING;
		}
		else {
			g_ovs_init->stopEnroll();
			if (destroy_enroll_c(s_device_id) != OVS_SUCCESS) {
				ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API - Enroll destroy is failed\n");
				return OVS_ERR_INVALID_DEVICE_INDEX;
			}
			g_ovs_init->destroyEnroll(s_device_id);
		}
	}
	
	if (g_ovs_init->isAliveFeature() == true) {
		if (stop_feature_c(s_device_id) != OVS_SUCCESS) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API - Feature stop is failed\n");
			return OVS_ERR_CAPTURE_IS_NOT_RUNNING;
		}
		else {
			g_ovs_init->stopFeature();
			if (destroy_feature_c(s_device_id) != OVS_SUCCESS) {
				ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API - Feature destroy is failed\n");
				return OVS_ERR_INVALID_DEVICE_INDEX;
			}
			g_ovs_init->destroyFeature(s_device_id);
		}
	}

	g_ovs_serial->ledControl("S1");
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AbortCapture API finished (res:%d)\n", ovs_res);
	return ovs_res;
}
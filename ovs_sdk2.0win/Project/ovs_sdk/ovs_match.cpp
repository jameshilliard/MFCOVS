/**
*  Matcher Functions Declaration
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

#include "whois_match.h"
#include "whois_feature_c.h"

#include "ovs_internal.h"
#include "ovs_api.h"


OVS_STATUS OVSSDK_API OVS_MatchVerify(const char* user_key, const char* user_fea)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchVerify API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchVerify API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}

	int ovs_res = OVS_SUCCESS;
	int user_id = g_ovs_userinfos->findUserId(user_key);
	if (user_id > 0) {
		if (match_1v1(user_id, user_fea, g_ovs_cfg.matchingLevel, OVSIN_HAND_TYPE_DEFAULT) == true) {
			ovs_res = OVS_SUCCESS;
			g_ovs_serial->ledControl("G1");
			g_ovs_serial->beepControl("M1");
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchVerify API - Verify Success\n");
		}
		else {
			ovs_res = OVS_ERR_MATCHING_FAILED;
			g_ovs_serial->ledControl("R1");
			g_ovs_serial->beepControl("M3");
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchVerify API - Verify Failed\n");
		}
	}
	else {
		ovs_res = OVS_ERR_MATCHING_FAILED;
		g_ovs_serial->ledControl("R1");
		g_ovs_serial->beepControl("M3");
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchVerify API - Verify Failed\n");
	}
	g_ovs_init->stopFeature();
	OVS_AbortCapture();
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchVerify API finished (res:%d)\n", ovs_res);
	return ovs_res;
}

#include <ppl.h>
OVS_STATUS OVSSDK_API OVS_MatchIdentify(const char* user_fea, int timeout, char* matched_key)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (timeout != 0 && g_ovs_timer->getElapsedTime() > timeout) {
		g_ovs_serial->ledControl("R1");
		g_ovs_serial->beepControl("M3");
		g_ovs_init->stopFeature();
		g_ovs_init->captureDone();
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - In identify, matching is timeout\n");
		return OVS_ERR_MATCHING_TIMEOUT;
	}
	if (user_fea == NULL || matched_key == NULL) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - Invalid parameter of timeout(%d)\n", timeout);
		return OVS_ERR_INVALID_PARAM;
	}
	int pid = partition_count();
	int *uids = new int[pid];
	Concurrency::parallel_for(0, pid, [&](int i) {
		uids[i] = match_1vN(user_fea, i);
	});
	
	int ovs_res = OVS_SUCCESS;
	int matched_id = -1;
	int uid_cnt = 0;
	for (int i = 0; i < pid; i++) {
		if (uids[i] > 0) {
			uid_cnt++;
			matched_id = uids[i];
		}
	}

	if (uid_cnt > 1) {
		Concurrency::parallel_for(0, pid, [&](int i) {
			if (uids[i] > 0 &&
				match_1v1(uids[i], user_fea, OVSIN_IDENTIFY_2ND_MATCHING_LEVEL, OVSIN_HAND_TYPE_DEFAULT) == true) {
				matched_id = uids[i];
				ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - In identify, 2nd matching is succeed (uid:%d)\n", matched_id);
			}
		});
		g_ovs_serial->ledControl("G1");
		g_ovs_serial->beepControl("M1");
	}
	else if (uid_cnt == 1) {
		g_ovs_serial->ledControl("G1");
		g_ovs_serial->beepControl("M1");
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - In identify, 1st matching is succeed (uid:%d)\n", matched_id);
	}
	else {
		ovs_res = OVS_ERR_MATCHING_FAILED;
		g_ovs_serial->ledControl("R1");
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - In identify, matching is failed\n");
	}

	if (matched_id > 0) {
		g_ovs_init->stopFeature();
		g_ovs_init->captureDone();
		std::string find_key_str = g_ovs_userinfos->findUserKey(matched_id);
		if (matched_key != NULL && !find_key_str.empty()) {
			const char* find_key = find_key_str.c_str();
			strcpy(matched_key, find_key);
		}
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API - Result (id:%d, key:%s)\n", matched_id, matched_key);
	}

	delete[] uids;
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_MatchIdentify API finished\n");
	return ovs_res;
}
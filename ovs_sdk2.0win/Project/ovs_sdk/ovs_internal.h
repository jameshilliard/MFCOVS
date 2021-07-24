/**
*  Development Functions Definition
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Internal Class Definition
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

#ifndef __OVS_INTERNAL_H__
#define __OVS_INTERNAL_H__

#include "ovs_util.h"
#include "ovs_serial.h"
#include "ovs_error.h"
#include "ovs_data.h"
#include "whois_match.h"
#include "whois_feature_c.h"

#define OVSIN_MAX_USER_IN_ONE_PARTITION		20000
#define OVSIN_HAND_TYPE_DEFAULT				0
#define OVSIN_THREAD_NUM_DEFAULT			0
#define OVSIN_IDENTIFY_2ND_MATCHING_LEVEL	80

typedef enum {
	OVSIN_ENROLL_STAGE = 0x0001,
	OVSIN_ENROLL_CANDIDATE = 0x0002,
	OVSIN_ENROLL_SUCCEED = 0x0003,
	OVSIN_ENROLL_FAIL = 0x0004,
	OVSIN_ENROLL_PALM_OUT = 0x0005,
} OVSInEnrollNotify;

typedef enum{
	OVSIN_FEATURE_GET_OK = 0x0100,
	OVSIN_FEATURE_GET_ERROR = 0x0200,
	OVSIN_FEATURE_PALM_IN = 0x0300,
	OVSIN_FEATURE_PALM_OUT = 0x0400,
} OVSInFeatureNotify;

class COvsInit
{
public:
	COvsInit() { Reset(); }
	~COvsInit() { }

	void Reset() {
		_bOpen = false;
		_bStartEnroll = false;
		_bCreateEnroll = false;
		_bStartFeature = false;
		_bCreateFeature = false;
		_bCaptureDone = false;
	}

	bool isAliveDevice(void) { return _bOpen; }
	void openDevice() { _bOpen = true; }

	bool isStatusEnroll(void) { return _bStartEnroll; }
	void stopEnroll() { _bStartEnroll = false; }
	void startEnroll() { _bStartEnroll = true; }

	bool isStatusFeature(void) { return _bStartFeature; }
	void stopFeature() { _bStartFeature = false; }
	void startFeature() { _bStartFeature = true; }

	bool isAliveEnroll(void) { return _bCreateEnroll; }
	int createEnroll(const char* did) { 
		if (create_enroll_c(did) != OVS_SUCCESS) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "CreateEnroll - Cannot create enroll event\n");
			return OVS_ERROR;
		}
		_bCreateEnroll = true;
		return OVS_SUCCESS;
	}
	void destroyEnroll(const char* did) {
		stop_enroll_c(did);
		destroy_enroll_c(did);
		_bCreateEnroll = false;
	}

	bool isAliveFeature(void) { return _bCreateFeature; }
	int createFeature(const char* did) {
		if (create_feature_c(did) != OVS_SUCCESS) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "CreateFeature - Cannot create enroll event\n");
			return OVS_ERROR;
		}
		_bCreateFeature = true;
		return OVS_SUCCESS;
	}
	void destroyFeature(const char* did) {
		stop_feature_c(did);
		destroy_feature_c(did);
		_bCreateFeature = false; 
	}

	bool isCaptureDone() { return _bCaptureDone; }
	void captureDone() { _bCaptureDone = true; }
	void captureResume() { _bCaptureDone = false; }

private:
	bool _bOpen;
	bool _bStartEnroll;
	bool _bCreateEnroll;
	bool _bStartFeature;
	bool _bCreateFeature;
	bool _bCaptureDone;
};

//////////////////////////////////////////////////////////////////////////
// ovs_comm(c# library) is replaced to c++ source code
//////////////////////////////////////////////////////////////////////////
// #import <mscoree.tlb> raw_interfaces_only
// #ifdef WIN64
// #import "../../bin/x64/ovs_comm.tlb" no_namespace named_guids
// #else //WIN64
// #import "../../bin/ovs_comm.tlb" no_namespace named_guids
// #endif //WIN64
// class COvsSerial {
// private:
// 	IOvs_Comm *_pOvsComm = NULL;
// 
// 	void wgdControl(const char* msg) {
// 		_pOvsComm->OVS_SendMessage("WIEGAND", msg);
// 	}
// 
// 	void rs232Control(const char* msg) {
// 		_pOvsComm->OVS_SendMessage("RS232", msg);
// 	}
// 
// 	void rs485Control(const char* msg) {
// 		_pOvsComm->OVS_SendMessage("RS485", msg);
// 	}
// 
// 	void relayControl(const char* msg) {
// 		_pOvsComm->OVS_SendMessage("RELAY", msg);
// 	}
// 
// public:
// 	int openSerial(void)
// 	{
// 		CoInitialize(NULL);
// 		HRESULT hr = CoCreateInstance(CLSID_ovs_comm, NULL, CLSCTX_INPROC_SERVER, IID_IOvs_Comm, reinterpret_cast<void**>(&_pOvsComm));
// 		if (SUCCEEDED(hr)) {
// 			int ovs_res = OVS_SUCCESS;
// 			if ((ovs_res = _pOvsComm->OVS_InitComm("Q!ngDa0N@ve1&e*mT2ch.?")) == OVS_ERR_NO_DEVICE) {
// 				if (ovs_license::isValid()) {
// 					ovs_res = OVS_SUCCESS;
// 				}
// 				else {
// 					ovs_res = OVS_ERR_INVALID_LICENSE;
// 				}
// 			}
// 			return ovs_res;
// 		}
// 		else {
// 			return OVS_ERR_NO_DEVICE;
// 		}
// 	}
// 
// 	void ledControl(const char* ledColor) {
// 		_pOvsComm->OVS_SendMessage("LED", ledColor);
// 	}
// };
//////////////////////////////////////////////////////////////////////////

#include <hash_map>
class COvsUserInfos
{
public:
	COvsUserInfos()
	{
		_user_count = 1;
		_user_ids = new stdext::hash_map<int, std::string>();
		_user_keys = new stdext::hash_map<std::string, int>();
	}
	~COvsUserInfos()
	{
		_user_count = 1;
		delete[] _user_ids;
		delete[] _user_keys;
	}

	void addUser(int user_id, std::string user_key)
	{
		_user_ids->insert(stdext::hash_map<int, std::string>::value_type(user_id, user_key));
		_user_keys->insert(stdext::hash_map<std::string, int>::value_type(user_key, user_id));
		_user_count++;
	}

	void deleteUser(int user_id, std::string user_key)
	{
		_user_ids->erase(user_id);
		_user_keys->erase(user_key);
	}

	void deleteAll()
	{
		_user_ids->erase(_user_ids->begin(), _user_ids->end());
		_user_keys->erase(_user_keys->begin(), _user_keys->end());
	}

	int getCurrId()
	{
		return _user_count;
	}

	int findUserId(std::string user_key)
	{
		stdext::hash_map<std::string, int>::iterator findIter = _user_keys->find(user_key);
		if (findIter != _user_keys->end())
		{
			return findIter->second;
		}
		else
		{
			return -1;
		}
	}

	std::string findUserKey(int user_id)
	{
		stdext::hash_map<int, std::string>::iterator findIter = _user_ids->find(user_id);
		if (findIter != _user_ids->end())
		{
			return findIter->second;
		}
		else
		{
			return NULL;
		}
	}

private:
	stdext::hash_map<int, std::string> *_user_ids;
	stdext::hash_map<std::string, int> *_user_keys;
	int _user_count;
};

extern COvsSerial* g_ovs_serial;
extern COvsTimer* g_ovs_timer;
extern COvsInit* g_ovs_init;
extern COvsUserInfos* g_ovs_userinfos;
extern ovs_ini::OVS_CFG_t g_ovs_cfg;

static const char s_device_id[] = "";

static unsigned char* s_enrollimage_data = NULL;
static int s_enrollimage_width = 0;
static int s_enrollimage_height = 0;
static int s_enrollimage_size = 0;

static unsigned char* s_matchimage_data = NULL;
static int s_matchimage_width = 0;
static int s_matchimage_height = 0;
static int s_matchimage_size = 0;

static int s_enrollstage_value = 0;
static int s_enrollstage_candget = 0;

static void set_partition_options(const int pid)
{
	create_partition(pid);
	set_match_level(pid, g_ovs_cfg.matchingLevel);
	set_match_type(pid, OVSIN_HAND_TYPE_DEFAULT);
	set_thread_num(pid, OVSIN_THREAD_NUM_DEFAULT);
}

static int create_enroll_options(const char* did)
{
	if (set_enroll_dir_c("", did) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	if (set_enroll_type_c(g_ovs_cfg.enrollType, s_device_id) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	return OVS_SUCCESS;
}

static int destroy_enroll_options(const char* did)
{
	if (stop_enroll_c(did) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	if (destroy_enroll_c(did) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	return OVS_SUCCESS;
}

static int create_match_options(const char* did)
{
	if (set_feature_dir_c("", did) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	return OVS_SUCCESS;
}

static int destroy_match_options(const char* did)
{
	if (stop_feature_c(did) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	if (destroy_feature_c(did) != OVS_SUCCESS) {
		return OVS_ERROR;
	}
	return OVS_SUCCESS;
}

#define OVSIN_SUITABLE_MIN_POSX 300
#define OVSIN_SUITABLE_MAX_POSX 400
#define OVSIN_SUITABLE_MIN_POSY 160
#define OVSIN_SUITABLE_MAX_POSY 260
#define OVSIN_STABLE_MIN_POSX 325
#define OVSIN_STABLE_MAX_POSX 375
#define OVSIN_STABLE_MIN_POSY 185
#define OVSIN_STABLE_MAX_POSY 235
static int is_stable_palm(int pos_cx, int pos_cy)
{
	if (pos_cx > OVSIN_STABLE_MIN_POSX && pos_cx < OVSIN_STABLE_MAX_POSX && 
		pos_cy > OVSIN_STABLE_MIN_POSY && pos_cy < OVSIN_STABLE_MAX_POSY)
	{
		return OVS_GET_PALM_STABLE_GOOD;
	}
	else if (pos_cx > OVSIN_SUITABLE_MIN_POSX && pos_cx < OVSIN_SUITABLE_MAX_POSX && 
		pos_cy > OVSIN_SUITABLE_MIN_POSY && pos_cy < OVSIN_SUITABLE_MAX_POSY)
	{
		return OVS_GET_PALM_STABLE_NORMAL;
	}
	else
	{
		return OVS_GET_PALM_STABLE_BAD;
	}
}

#endif //__OVS_INTERNAL_H__
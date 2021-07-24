/**
*  Enrollment Functions Declaration
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

#include "ovs_internal.h"
#include "ovs_api.h"


OVS_STATUS OVSSDK_API OVS_UserCount(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UserCount API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UserCount API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	unsigned int user_count = user_count_in_lib();
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UserCount (user:%d) API finished\n", user_count);
	return user_count;
}

OVS_STATUS OVSSDK_API OVS_AddUser(const char* user_key, const char* left_fea, const char* right_fea)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (user_key == NULL || (left_fea == NULL && right_fea == NULL)) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Invalid parameters\n");
		return OVS_ERR_INVALID_PARAM;
	}

	int ovs_res = OVS_SUCCESS;
	if (g_ovs_userinfos->findUserId(user_key) >= 0) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Already exist user\n");
		ovs_res = OVS_ERR_ALREADY_EXIST_USER;
	}
	else {
		int user_id = g_ovs_userinfos->getCurrId();
		if (add_user_to_lib(user_id, left_fea, right_fea) == false) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Failed to add user in lib\n");
			ovs_res = OVS_ERR_FAILED_TO_ADD_USER;
		}
		else {
			unsigned int pid = partition_count() - 1;
			if (pid < 0) { pid = 0; set_partition_options(pid); }
			if (user_count_in_partition(pid) >= OVSIN_MAX_USER_IN_ONE_PARTITION) {
				set_partition_options(++pid);
				ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Increased partition id (%d)\n", pid);
			}
			if (assign_user_to_partition(user_id, pid) == false) {
				ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Failed to add user(ID: %d, KEY: %s) in partition (%d)\n", user_id, user_key, pid);
				ovs_res = OVS_ERR_FAILED_TO_ADD_USER;
			}
			else {
				ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API - Succeed to add user(ID: %d, KEY: %s) in partition (%d)\n", user_id, user_key, pid);
				g_ovs_userinfos->addUser(user_id, user_key);
				ovs_res = OVS_SUCCESS;
			}
		}
	}
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_AddUser API finished (res:%d)\n", ovs_res);
	return ovs_res;
}

OVS_STATUS OVSSDK_API OVS_UpdateUser(const char* user_key, const char* left_fea, const char* right_fea)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (user_key == NULL || (left_fea == NULL && right_fea == NULL)) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API - Invalid parameters\n");
		return OVS_ERR_INVALID_PARAM;
	}

	int ovs_res = OVS_SUCCESS;
	int user_id = g_ovs_userinfos->findUserId(user_key);
	if (user_id < 0) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API - No exist user\n");
		ovs_res = OVS_ERR_NO_EXIST_USER;
	}
	else {
		if (update_user_in_lib(user_id, left_fea, right_fea) == false) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API - Failed to update user\n");
			ovs_res = OVS_ERR_FAILED_TO_UPDATE_USER;
		}
		else {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API - Succeed to update user\n");
			ovs_res = OVS_SUCCESS;
		}
	}
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_UpdateUser API finished (res:%d)\n", ovs_res);
	return ovs_res;
}

OVS_STATUS OVSSDK_API OVS_DeleteUser(const char* user_key)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	if (user_key == NULL) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API - Invalid parameters\n");
		return OVS_ERR_INVALID_PARAM;
	}

	int ovs_res = OVS_SUCCESS;
	int user_id = g_ovs_userinfos->findUserId(user_key);
	if (user_id < 0) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API - No exist user\n");
		ovs_res = OVS_ERR_NO_EXIST_USER;
	}
	else {
		if (delete_user_from_lib(user_id) == false) {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API - Failed to delete user\n");
			ovs_res = OVS_ERR_FAILED_TO_DELETE_USER;
		}
		else {
			ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API - Succeed to delete user\n");
			g_ovs_userinfos->deleteUser(user_id, user_key);
			ovs_res = OVS_SUCCESS;
		}
	}
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteUser API finished (res:%d)\n", ovs_res);
	return ovs_res;
}

OVS_STATUS OVSSDK_API OVS_DeleteAllUsers(void)
{
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteAllUsers API called\n");
	if (g_ovs_init == NULL || g_ovs_init->isAliveDevice() == false) {
		ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteAllUsers API - Not initialized\n");
		return OVS_ERR_SDK_UNINITIALIZED;
	}
	delete_all_users_from_lib();
	clear_partitions();
	set_partition_options(0);
	g_ovs_userinfos->deleteAll();
	int ovs_res = OVS_SUCCESS;
	if (user_count_in_lib() > 0) {
		ovs_res = OVS_ERR_FAILED_TO_DELETE_USER;
	}
	ovs_debug::logDebug(3, OVS_DEBUG_LOG_PROCESS_API, "OVS_DeleteAllUsers API finished\n");
	return ovs_res;
}
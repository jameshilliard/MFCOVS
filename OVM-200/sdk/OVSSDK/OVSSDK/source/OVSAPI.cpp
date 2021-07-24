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
#include "OVSAPI.h"
#include "OVSManager.h"

shared_ptr<OVSManager> p_ovs_manager;

OVSSTATUS OVSSDK_API OVS_InitSDK(void)
{
	// 设备初始化
	if (p_ovs_manager != nullptr)
	{
		return SUCCESS;
	}
	p_ovs_manager = make_shared<OVSManager>();
	return p_ovs_manager->OVSInit();
}

OVSSTATUS OVSSDK_API OVS_Set_Exposure(const int val)
{
	if (p_ovs_manager->p_ovs_device->Set_Exposure(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Get_Exposure(int& val)
{
	if (p_ovs_manager->p_ovs_device->Get_Exposure(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Set_LEDCurrent(const int val)
{
	if (p_ovs_manager->p_ovs_device->Set_LEDCurrent(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Get_LEDCurrent(int& val)
{
	if (p_ovs_manager->p_ovs_device->Get_LEDCurrent(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Set_BLOpen(const bool flag)
{
	if (p_ovs_manager->p_ovs_device->Enable_BL(flag) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Get_BL(int& val)
{
	/*if (p_ovs_manager->p_ovs_device->Get_BL(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;*/
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_Set_Gain(int val)
{
	if (p_ovs_manager->p_ovs_device->Set_Gain(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Get_Gain(int& val)
{
	if (p_ovs_manager->p_ovs_device->Get_Gain(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;

}

OVSSTATUS OVSSDK_API OVS_Set_CompressionRatio(int val)
{
	if (p_ovs_manager->p_ovs_device->Set_CompressionRatio(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Get_CompressionRatio(int& val)
{
	if (p_ovs_manager->p_ovs_device->Get_CompressionRatio(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_Get_Distance(int& val)
{
	if (p_ovs_manager->p_ovs_device->Get_Distance(val) == S_OK)
	{
		return SUCCESS;
	}
	return ERR;
}

OVSSTATUS OVSSDK_API OVS_UninitSDK(void)
{

	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_StartFeatureForEnroll()
{
	p_ovs_manager->StartFeatureForEnroll();
	return SUCCESS;
}


OVSSTATUS OVSSDK_API OVS_StartFeatureForMatch()
{
	p_ovs_manager->StartFeatureForMatch();
	return SUCCESS;
}


OVSSTATUS OVSSDK_API OVS_GetPreviewImage(Image& image, Information& info)
{
	p_ovs_manager->GetPreviewImage(image, info);

	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_GetPreviewImageForEnroll(Image& image)
{
	p_ovs_manager->GetPreviewImageForEnroll(image);

	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_AbortCapture()
{

	if (p_ovs_manager->AbortCaptureRegister() != SUCCESS)
	{
		return ERR;
	}
	if (p_ovs_manager->AbortCaptureIdenfiy() != SUCCESS)
	{
		return ERR; 
	}
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_GetEnrollFeature(string& feature)
{
	return p_ovs_manager->GetEnrollFeature(feature);
}

OVSSTATUS OVSSDK_API OVS_GetMatchFeature(string& feature)
{
	return p_ovs_manager->GetMatchFeature(feature);
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_MatchVerify(const std::string featureSrc, const std::string featureDest)
{
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_MatchIdentify(string& feature, string& userID)
{
	p_ovs_manager->MatchUser(feature, userID);
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_UserCount()
{

	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_AddUser(string& userID, string& featureLeft, string& featureRight)
{
	p_ovs_manager->AddUserSingleInfo(userID, featureLeft, featureRight);
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_UpdateUser(const std::string userID, const std::string featureLeft, const std::string featureRight)
{
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_DeleteUser(const std::string userID)
{
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_DeleteAllUsers()
{
	return SUCCESS;
}

OVSSTATUS OVSSDK_API OVS_GetErrorString(const int& errorCode)
{
	return SUCCESS;
}

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

#ifndef __OVS_DEVICE_H__
#define __OVS_DEVICE_H__

//#ifdef WIN32
#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "SonixCamera_win.h"
#include "AudioVideo.h"
#include "MultiCamera.h"
using namespace std;
//#endif //WIN32

typedef enum _ENUM_DEVICE_TYPE
{
	TYPE_COMPORT_NAME = 0x00,
	TYPE_COMPORT_VIDPID,
	TYPE_USB_NAME,
	TYPE_USB_VIDPID,
} ENUM_DEVICE_TYPE;

class OVSDevice
{
private://register address
	unsigned short COMS_IMAGE_STREAM_ADDR = 0x0C83;

	unsigned short COMS_COMPRESSIONRATIO_ADDR = 0x0C84;

	unsigned short COMS_EXP_ADDR_H8 = 0x3500;
	unsigned short COMS_EXP_ADDR_M8 = 0x3501;
	unsigned short COMS_EXP_ADDR_L8 = 0x3502;

	unsigned short COMS_GAIN_ADDR_H1 = 0x350A;
	unsigned short COMS_GAIN_ADDR_L8 = 0x350B;

	unsigned short COMS_BLACK_LEVEL_ADDR = 0x4007;

	unsigned short DSP_FUNCTION_INTERRUPT_ADDR = 0x1003;

	unsigned short DSP_TO_DISTANCE_ADDR = 0x0C8A;
	unsigned short DSP_TO_DISTANCE_REFLECT_L_ADDR = 0x0C8B;
	unsigned short DSP_TO_DISTANCE_REFLECT_H_ADDR = 0x0C8C;

	unsigned short DSP_TO_DISTANCE_FUN_ADDR = 0x0C8F;

	unsigned short DSP_TO_COMS_ADDR_H = 0x0C90;
	unsigned short DSP_TO_COMS_ADDR_L = 0x0C91;
	unsigned short DSP_TO_COMS_ADDR_D = 0x0C92;
	unsigned short DSP_TO_COMS_ADDR_T = 0x0C93;

	unsigned short DSP_TO_LED_ADDR = 0x0C94;
public:

	explicit OVSDevice();
    ~OVSDevice();
    static OVSDevice *instance();

	bool m_device_status = false;
	bool DeviceInit();

	HRESULT OpenDevice();
	HRESULT CloseDevice();



	HRESULT Set_CompressionRatio(int val);
	HRESULT	Get_CompressionRatio(int& val);

	HRESULT Set_Exposure(int val);
	HRESULT Get_Exposure(int& val);

	HRESULT Set_Gain(int val);
	HRESULT Get_Gain(int& val);

	HRESULT Set_LEDCurrent(int val);
	HRESULT Get_LEDCurrent(int& val);

	HRESULT Set_BLOpen(bool flag);

	HRESULT Get_Distance(int& val);
	HRESULT Get_Distance_Ref_L(int& val);
	HRESULT Get_Distance_Ref_H(int& val);

	HRESULT Enable_Distance_Function(bool enable);
	HRESULT Enable_DSP_Function(bool enable);
private:
	HRESULT AsicRegisterWrite(unsigned short addr, string val, int len);
	HRESULT AsicRegisterRead(unsigned short addr, string& val, int len);


	HRESULT COMS_Gain_High_1_bit_Write(int val);
	HRESULT COMS_Gain_Low_8_bit_Write(int val);
	HRESULT COMS_Gain_Low_8_bit_Read(int& val);

	HRESULT COMS_Exposure_High_8_bit_Write(int val);
	HRESULT COMS_Exposure_Middle_8_bit_Write(int val);
	HRESULT COMS_Exposure_Middle_8_bit_Read(int& val);
	HRESULT COMS_Exposure_Low_8_bit_Write(int val);

	void Dec2Hex(int x, string& in_str);
	void Dec2HexK(int x, string& in_str);
};
#endif //__OVS_DEVICE_H__



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
#include "OVSDevice.h"

OVSDevice::OVSDevice()
{
}

OVSDevice::~OVSDevice()
{
	CloseDevice();
	SonixCam_UnInit();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�豸��ʼ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		DeviceInit
*	Description:
*	Parameters:		��
*	Return :		��ʼ���ɹ�����	true, ��ʼ��ʧ�ܷ���:false
*/
bool OVSDevice::DeviceInit()
{
	HRESULT ret = NOERROR;
	ret = SonixCam_Init();
	if (ret != S_OK)
	{
		printf("ERROR: SonixCam_Init() - Device Init false!\n");
		return false;
	}

	//�ȴ�ͼ����
	ret = OpenDevice();
	if (ret != S_OK)
	{
		printf("ERROR: OpenDevice() - Open Device false!\n");
		return false;
	}
	//��ʼ������
	ret = Set_LEDCurrent(0);
	if (ret != S_OK)
	{
		printf("ERROR: Set_LEDCurrent() - Current Write false!\n");
		return false;
	}
	//��ʼ���ع�
	ret = COMS_Exposure_High_8_bit_Write(0);
	if (ret != S_OK)
	{
		printf("ERROR: COMS_Exposure_High_8_bit_Write() - COMS Exposure High 8bit Write false!\n");
		return false;
	}
	ret = COMS_Exposure_Middle_8_bit_Write(28);
	if (ret != S_OK)
	{
		printf("ERROR: COMS_Exposure_Middle_8_bit_Write() - COMS Exposure Middle 8bit Write false!\n");
		return false;
	}
	ret = COMS_Exposure_Low_8_bit_Write(0);
	if (ret != S_OK)
	{
		printf("ERROR: COMS_Exposure_Low_8_bit_Write() - COMS Exposure Low 8bit Write false!\n");
		return false;
	}
	//��ʼ������
	ret = COMS_Gain_High_1_bit_Write(0);
	if (ret != S_OK)
	{
		printf("ERROR: COMS_Gain_High_1_bit_Write() - COMS Gain High 1bit Write false!\n");
		return false;
	}
	ret = COMS_Gain_Low_8_bit_Write(16);
	if (ret != S_OK)
	{
		printf("ERROR: COMS_Gain_Low_8_bit_Write() - COMS Gain Low 8bit Write false!\n");
		return false;
	}
	//��ʼ��ѹ����
	ret = Set_CompressionRatio(5);
	if (ret != S_OK)
	{
		printf("ERROR: Set_CompressionRatio() - COMS Compression Ratio Set false!\n");
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�����豸
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		OpenDevice
*	Description:
*	Parameters:		��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/

HRESULT OVSDevice::OpenDevice()
{
	HRESULT ret = NOERROR;
	//ret = Enable_Distance_Function(true);
	//if (ret != S_OK)
	//{
	//	return ret;
	//}
	ret = AsicRegisterWrite(COMS_IMAGE_STREAM_ADDR, "01", 1);
	//if (ret != S_OK)
	//{
	//	return ret;
	//}
	return ret;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�ر��豸
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		CloseDevice
*	Description:
*	Parameters:		��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/

HRESULT OVSDevice::CloseDevice()
{
	HRESULT ret = NOERROR;
	ret = AsicRegisterWrite(COMS_IMAGE_STREAM_ADDR, "00", 1);
	return ret;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ͼ��ѹ����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_CompressionRatio
*	Description:
*	Parameters:		Val��ѹ����ֵ��5-255����Ĭ��ֵ5
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Set_CompressionRatio(int val)
{
	HRESULT ret = NOERROR;
	string set_val;
	
	Dec2Hex(val, set_val);

	ret = AsicRegisterWrite(COMS_COMPRESSIONRATIO_ADDR, set_val, 1);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��ȡ��ǰͼ��ѹ����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_CompressionRatio
*	Description:
*	Parameters:		Val��ѹ����ֵ��5-255��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_CompressionRatio(int& val)
{
	HRESULT ret = NOERROR;
	string set_val;
	ret = AsicRegisterRead(COMS_COMPRESSIONRATIO_ADDR, set_val, 1);
    val = atoi(set_val.c_str());
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	дCOMS���棨��1λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Gain_High_1_bit_Write
*	Description:
*	Parameters:		Val������ֵ��0x00-0x01����Ĭ��ֵ0����ʼ���Ѿ��趨���������������趨��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Gain_High_1_bit_Write(int val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_GAIN_ADDR_H1 >> 8;
	Addr_L = COMS_GAIN_ADDR_H1 & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	Dec2Hex(val, set_val);
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, set_val, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	дCOMS���棨��8λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Gain_Low_8_bit_Write
*	Description:
*	Parameters:		Val������ֵ��0x00-0xff����Ĭ��ֵ0x10��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Gain_Low_8_bit_Write(int val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_GAIN_ADDR_L8 >> 8;
	Addr_L = COMS_GAIN_ADDR_L8 & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	Dec2Hex(val, set_val);
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, set_val, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰCOMS���棨��8λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Gain_Low_8_bit_Write
*	Description:
*	Parameters:		Val����ȡ������ֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Gain_Low_8_bit_Read(int& val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_GAIN_ADDR_L8 >> 8;
	Addr_L = COMS_GAIN_ADDR_L8 & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "00", 1);
	if (ret != S_OK)
	{
		return ret;
	}

	ret = AsicRegisterRead(DSP_TO_COMS_ADDR_D, set_val, 1);
	val = atoi(set_val.c_str());
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	дCOMS�ع⣨��8λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Exposure_High_8_bit_Write
*	Description:
*	Parameters:		Val���ع�ֵ��0x00-0xff����Ĭ��ֵ0x00���ع�ֵ����0x00��ͼ�����˸��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Exposure_High_8_bit_Write(int val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = (COMS_EXP_ADDR_H8 & 0xFF00) >> 8;
	Addr_L = (COMS_EXP_ADDR_H8 & 0x00FF) << 8;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	Dec2Hex(val, set_val);
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, set_val, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	дCOMS�ع⣨��8λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Exposure_Middle_8_bit_Write
*	Description:
*	Parameters:		Val���ع�ֵ��0x00-0xff����Ĭ��ֵ0x28���ع�ֵ����0x44��ͼ�����˸��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Exposure_Middle_8_bit_Write(int val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_EXP_ADDR_M8 >> 8;
	Addr_L = COMS_EXP_ADDR_M8 & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	Dec2Hex(val, set_val);
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, set_val, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰCOMS�ع⣨��8λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Exposure_Middle_8_bit_Read
*	Description:
*	Parameters:		Val���ع�ֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Exposure_Middle_8_bit_Read(int& val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_EXP_ADDR_M8 >> 8;
	Addr_L = COMS_EXP_ADDR_M8 & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "00", 1);
	if (ret != S_OK)
	{
		return ret;
	}

	ret = AsicRegisterRead(DSP_TO_COMS_ADDR_D, set_val, 1);
	val = atoi(set_val.c_str());
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	дCOMS�ع⣨��8λ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		COMS_Exposure_Low_8_bit_Write
*	Description:
*	Parameters:		Val���ع�ֵ��0x00-0xff����Ĭ��ֵ0x00������Ч�������ԣ����Բ����е��ڣ�
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::COMS_Exposure_Low_8_bit_Write(int val)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_EXP_ADDR_L8 >> 8;
	Addr_L = COMS_EXP_ADDR_L8 & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	string set_val;
	Dec2Hex(val, set_val);
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, set_val, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}

void OVSDevice::Dec2Hex(int x, string& in_str)
{
	stringstream ss;
	ss << setw(2) << setfill('0') << hex << x;
	ss >> in_str;
	transform(in_str.begin(), in_str.end(), in_str.begin(), ::toupper);
}

void OVSDevice::Dec2HexK(int x, string& in_str)
{
	stringstream ss;
	ss << setw(2) << setfill('0') << hex << x;
	ss >> in_str;
	in_str = "\r\n" + in_str;
	transform(in_str.begin(), in_str.end(), in_str.begin(), ::toupper);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����COMS�ع�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_Exposure
*	Description:
*	Parameters:		Val���ع�ֵ��0x00-0xff����Ĭ��ֵ0x28���ع�ֵ����0x44��ͼ�����˸��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Set_Exposure(int val) //ֻ������8λ
{
	HRESULT ret = NOERROR;
	ret = Enable_Distance_Function(false);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = COMS_Exposure_Middle_8_bit_Write(val);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = Enable_Distance_Function(true);
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
	//return COMS_Exposure_Middle_8_bit_Write(val);		
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰCOMS�ع�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_Exposure
*	Description:
*	Parameters:		Val���ع�ֵ��0x00-0xff����
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_Exposure(int& val)
{
	return COMS_Exposure_Middle_8_bit_Read(val);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����COMS����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_Exposure
*	Description:
*	Parameters:		Val���ع�ֵ��0x00-0xff����Ĭ��ֵ0x10��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Set_Gain(int val)//ֻ���ڵ�8λ
{
	return COMS_Gain_Low_8_bit_Write(val);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰCOMS����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_Exposure
*	Description:
*	Parameters:		Val���ع�ֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_Gain(int& val)
{
	return COMS_Gain_Low_8_bit_Read(val);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����LED����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_LEDCurrent
*	Description:
*	Parameters:		Val������ֵ������ֵ����ʵ�ĵ���ֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Set_LEDCurrent(int val)
{
	HRESULT ret = NOERROR;
	string set_val;
	Dec2Hex(val, set_val);
	ret = AsicRegisterWrite(DSP_TO_LED_ADDR, set_val, 1);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰLED����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Get_LEDCurrent
*	Description:
*	Parameters:		Val������ֵ������ֵ����ʵ�ĵ���ֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_LEDCurrent(int& val)
{
	HRESULT ret = NOERROR;
	string set_val;
	ret = AsicRegisterRead(DSP_TO_LED_ADDR, set_val, 1);
	val = atoi(set_val.c_str());
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���úڵ�ƽֵ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Set_BL
*	Description:
*	Parameters:		Val���ڵ�ƽֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Enable_BL(bool flag)
{
	HRESULT ret = NOERROR;
	unsigned short Addr_H;
	unsigned short Addr_L;
	Addr_H = COMS_BLACK_LEVEL_ADDR >> 8;
	Addr_L = COMS_BLACK_LEVEL_ADDR & 0xff;
	string Addr_H_Str, Addr_L_Str;
	Dec2Hex(Addr_H, Addr_H_Str);
	Dec2Hex(Addr_L, Addr_L_Str);
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
	if (ret != S_OK)
	{
		return ret;
	}
	if (flag)
	{
		ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, "00", 1);
		if (ret != S_OK)
		{
			return ret;
		}
		ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
		if (ret != S_OK)
		{
			return ret;
		}
	}
	else
	{
		ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, "10", 1);
		if (ret != S_OK)
		{
			return ret;
		}
		ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
		if (ret != S_OK)
		{
			return ret;
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ʹ��DSP
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Enable_DSP_Function
*	Description:
*	Parameters:		True��������/ false(�ر�)��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Enable_DSP_Function(bool enable)
{
	HRESULT ret = NOERROR;
	if (enable)
	{
		ret = AsicRegisterWrite(DSP_FUNCTION_INTERRUPT_ADDR, "61", 1);
	}
	else
	{
		ret = AsicRegisterWrite(DSP_FUNCTION_INTERRUPT_ADDR, "60", 1);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ʹ�ܾ��봫����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Enable_Distance_Function
*	Description:
*	Parameters:		True��������/ false(�ر�)��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Enable_Distance_Function(bool enable)
{
	HRESULT ret = NOERROR;
	if (enable)
	{
		ret = AsicRegisterWrite(DSP_TO_DISTANCE_FUN_ADDR, "00", 1);
	}
	else
	{
		ret = AsicRegisterWrite(DSP_TO_DISTANCE_FUN_ADDR, "01", 1);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰ����ֵ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Get_Distance
*	Description:
*	Parameters:		Val������ֵ������ֵ����ʵ�ľ���ֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_Distance(int& val)
{
	HRESULT ret = NOERROR;
	string set_val;
	ret = AsicRegisterRead(DSP_TO_DISTANCE_ADDR, set_val, 1);
	val = atoi(set_val.c_str());
	if (ret != S_OK)
	{
		return ret;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰ���봫���������ʵͰ�λ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Get_Distance_Ref_L
*	Description:
*	Parameters:		Val�������ʵͰ�λֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_Distance_Ref_L(int& val)
{
	HRESULT ret = NOERROR;
	string set_val;
	ret = AsicRegisterRead(DSP_TO_DISTANCE_REFLECT_L_ADDR, set_val, 1);
	val = atoi(set_val.c_str());
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����ǰ���봫���������ʸ߰�λ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		Get_Distance_Ref_H
*	Description:
*	Parameters:		Val�������ʸ߰�λֵ��
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::Get_Distance_Ref_H(int& val)
{
	HRESULT ret = NOERROR;
	string set_val;
	ret = AsicRegisterRead(DSP_TO_DISTANCE_REFLECT_H_ADDR, set_val, 1);
	val = atoi(set_val.c_str());
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Dsp�Ĵ���д
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		AsicRegisterWrite
*	Description:
*	Parameters:		addr:dsp�Ĵ�����ַ��val����Ҫд������ݣ�len��Ҫд�������ֽ�����
*	Return :		�ɹ�����	S_OK, ʧ�ܷ���:NOERROR
*/
HRESULT OVSDevice::AsicRegisterWrite(unsigned short addr, string val, int len)
{
	HRESULT ret = NOERROR;
	BYTE* pData = new BYTE[len];
	memset(pData, 0xff, len);
	LONG sIndex = -1;
	string sTemp = val;
	LONG srcLength = val.length();
	BYTE add = 0;
	LONG i = 0;
	char temp[3] = { 0 };
	do {
		sIndex++;
		temp[add] = sTemp[sIndex];
		if (temp[add] == 0x20) {
			continue;
		}
		if (sTemp[sIndex] == '\r' && sTemp[sIndex + 1] == '\n')
		{
			sIndex++;
			continue;
		}
		add++;
		if (add == 2) {
			add = 0;
			pData[i++] = strtoul(temp, NULL, 16);
			temp[0] = 0;
			temp[1] = 0;
		}
	} while (sIndex < srcLength - 1);
	if (add)
		pData[i] = strtoul(temp, NULL, 10);
	ret = SonixCam_AsicRegisterWrite(addr, pData, len);
	delete[]pData;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Dsp�Ĵ�����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		AsicRegisterRead
*	Description:
*	Parameters:		addr[in]:dsp�Ĵ�����ַ��pData[out]�����������ݣ�len[in]�������������ֽ�����
*	Return :	    �ɹ�����	S_OK ��ʧ�ܷ���:NOERROR
*/

HRESULT OVSDevice::AsicRegisterRead(unsigned short addr, string& val, int len)
{
	HRESULT ret = NOERROR;
	BYTE* pData = new BYTE[len];
	memset(pData, 0xff, len);
	ret = SonixCam_AsicRegisterRead(addr, pData, len);
	val = to_string(pData[len-1]);
	//for (int i = 0; i < len; i++)
	//{
	//	if ((i % 16) == 0 && i != 0)
	//		Dec2HexK(pData[i], val);
	//	else
	//		Dec2Hex(pData[i], val);
	//}
	delete[]pData;
	return ret;
}

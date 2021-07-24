/**
*  Serial Communication Functions Definition
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Serial Class Definition
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

#ifndef __OVS_SERIAL_H__
#define __OVS_SERIAL_H__

#ifdef WIN32
#include <windows.h>
#include <string>
using namespace std;
#endif //WIN32


typedef enum _ENUM_DEVICE_TYPE
{
	TYPE_COMPORT_NAME = 0x00,
	TYPE_COMPORT_VIDPID,
	TYPE_USB_NAME,
	TYPE_USB_VIDPID,
} ENUM_DEVICE_TYPE;

class COvsSerial
{
public:
	COvsSerial();
	virtual ~COvsSerial();

private:
	bool OpenComPort(std::string portname);
	bool ConfigComPort(DWORD baudrate);
	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
		DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
		DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant);

	bool WriteByte(BYTE msg);
	bool WriteByte(const char* msg, int size);
	bool ReadByte(BYTE &msg);
	bool ReadByte(BYTE* &msg, UINT size);

	std::string FindDevice(const char* VID, const char* PID, ENUM_DEVICE_TYPE eENUM_DEVICE_TYPE);
	bool OpenDevice(const char* VID, const char* PID, int baudrate);

	HANDLE m_hComm;
	DCB m_dcb;
	COMMTIMEOUTS m_CommTimeouts;
	BOOL m_bPortReady;
	bool m_bOpenComm;

public:
	int openSerial();
	void closeSerial();
	bool isOpenSerial();
	void ledControl(const char* color);
	void beepControl(const char* mode);
};

#endif //__OVS_SERIAL_H__
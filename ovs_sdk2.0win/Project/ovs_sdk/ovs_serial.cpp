/**
*  Serial Communication Functions Declaration
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Serial API Declaration
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

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <tchar.h>

#include "ovs_serial.h"

#ifdef WIN32
#include <setupapi.h>
#pragma comment (lib, "Setupapi.lib")
#endif //WIN32


std::string COvsSerial::FindDevice(const char* VID, const char* PID, ENUM_DEVICE_TYPE eENUM_DEVICE_TYPE)
{
	std::string FriendlyName;
	GUID guid;
	HRESULT hr = S_FALSE;
	switch (eENUM_DEVICE_TYPE)
	{
	case ENUM_DEVICE_TYPE::TYPE_COMPORT_VIDPID:
	{
		wchar_t* clsid_str = L"{86E0D1E0-8089-11D0-9CE4-08003E301F73}";
		hr = CLSIDFromString(clsid_str, &guid);
		break;
	}
	case ENUM_DEVICE_TYPE::TYPE_USB_VIDPID:
	{
		wchar_t* clsid_str = L"{A5DCBF10-6530-11D2-901F-00C04FB951ED}";
		hr = CLSIDFromString(clsid_str, &guid);
		break;
	}
	default:
		throw;
		return "";
	}
	if (hr != S_OK)
	{
		return "";
	}

	HDEVINFO pHDEVINFO = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (pHDEVINFO == INVALID_HANDLE_VALUE)
		return "";
	DWORD dwIndex = 0;
	SP_DEVICE_INTERFACE_DATA sSP_DEVICE_INTERFACE_DATA = { sizeof(sSP_DEVICE_INTERFACE_DATA), };
	while (SetupDiEnumDeviceInterfaces(pHDEVINFO, NULL, &guid, dwIndex++, &sSP_DEVICE_INTERFACE_DATA))
	{
		DWORD dwSize = 0;
		if (!SetupDiGetDeviceInterfaceDetail(pHDEVINFO, &sSP_DEVICE_INTERFACE_DATA, NULL, 0, &dwSize, 0))
		{
			if (GetLastError() == ERROR_NO_MORE_ITEMS)
				break;
			PSP_DEVICE_INTERFACE_DETAIL_DATA pPSP_DEVICE_INTERFACE_DETAIL_DATA = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
			pPSP_DEVICE_INTERFACE_DETAIL_DATA->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			SP_DEVINFO_DATA sSP_DEVINFO_DATA = { sizeof(sSP_DEVINFO_DATA), };
			if (!SetupDiGetDeviceInterfaceDetail(pHDEVINFO, &sSP_DEVICE_INTERFACE_DATA, pPSP_DEVICE_INTERFACE_DETAIL_DATA, dwSize, &dwSize, &sSP_DEVINFO_DATA))
			{
				HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pPSP_DEVICE_INTERFACE_DETAIL_DATA);
				break;
			}

			TCHAR pszDeviceName[MAX_PATH] = { 0 };

			//Get VID & PID
			if (SetupDiGetDeviceRegistryProperty(pHDEVINFO, &sSP_DEVINFO_DATA, SPDRP_HARDWAREID, NULL, (PBYTE)pszDeviceName, 256, NULL)) {
				std::string DeviceName = std::string(pszDeviceName);
				if (DeviceName.find(VID) != std::string::npos && DeviceName.find(PID) != std::string::npos) {
					memset(pszDeviceName, 0, MAX_PATH);
					if (SetupDiGetDeviceRegistryProperty(pHDEVINFO, &sSP_DEVINFO_DATA, SPDRP_FRIENDLYNAME, NULL, (PBYTE)pszDeviceName, 256, NULL)) {
						char* comport = strrchr(pszDeviceName, '(');
						FriendlyName = std::string(comport);
						size_t findNum1 = FriendlyName.find("(");
						size_t findNum2 = FriendlyName.find(")");
						FriendlyName.erase(findNum1, findNum1 + 1);
						FriendlyName.erase(findNum2 - 1, findNum2);
					}
				}
			}
			HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pPSP_DEVICE_INTERFACE_DETAIL_DATA);
		}
	}
	SetupDiDestroyDeviceInfoList(pHDEVINFO);
	return FriendlyName;
}

bool COvsSerial::OpenComPort(std::string portname)
{
	std::string newPortName = "\\\\.\\" + portname;
	if ((m_hComm = CreateFile(newPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		return true;
	}
}

COvsSerial::COvsSerial()
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_bOpenComm = false;
}

COvsSerial::~COvsSerial()
{
	m_bOpenComm = false;
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool COvsSerial::OpenDevice(const char* VID, const char* PID, int baudrate)
{
	std::string PortName = FindDevice(VID, PID, ENUM_DEVICE_TYPE::TYPE_COMPORT_VIDPID);
	if (PortName.empty()) {
		return false;
	}

	if (OpenComPort(PortName)) {
		if (!ConfigComPort(baudrate)) {
			return false;
		}
		if (!SetCommunicationTimeouts(0, 0, 0, 0, 0)) {
			return false;
		}
		return true;
	}
	return false;
}

bool COvsSerial::ConfigComPort(DWORD baudrate)
{
	if ((m_bPortReady = GetCommState(m_hComm, &m_dcb)) == 0) {
		CloseHandle(m_hComm);
		return false;
	}

	m_dcb.BaudRate			= baudrate;
	m_dcb.ByteSize			= 8;
	m_dcb.Parity			= NOPARITY;
	m_dcb.StopBits			= ONESTOPBIT;
	m_dcb.fBinary			= true;
	m_dcb.fDsrSensitivity	= false;
	m_dcb.fParity			= false;
	m_dcb.fOutX				= false;
	m_dcb.fInX				= false;
	m_dcb.fNull				= false;
	m_dcb.fAbortOnError		= true;
	m_dcb.fOutxCtsFlow		= false;
	m_dcb.fOutxDsrFlow		= false;
	m_dcb.fDtrControl		= DTR_CONTROL_ENABLE;
	m_dcb.fDsrSensitivity	= false;
	m_dcb.fRtsControl		= RTS_CONTROL_ENABLE;

	if ((m_bPortReady = SetCommState(m_hComm, &m_dcb)) == 0) {
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool COvsSerial::SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
	DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
	DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant)
{
	if ((m_bPortReady = GetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0) {
		CloseHandle(m_hComm);
		return false;
	}

	m_CommTimeouts.ReadIntervalTimeout			= ReadIntervalTimeout;
	m_CommTimeouts.ReadTotalTimeoutConstant		= ReadTotalTimeoutConstant;
	m_CommTimeouts.ReadTotalTimeoutMultiplier	= ReadTotalTimeoutMultiplier;
	m_CommTimeouts.WriteTotalTimeoutConstant	= WriteTotalTimeoutConstant;
	m_CommTimeouts.WriteTotalTimeoutMultiplier	= WriteTotalTimeoutMultiplier;

	if ((m_bPortReady = SetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0) {
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool COvsSerial::WriteByte(BYTE msg)
{
	DWORD iBytesWritten = 0;
	if (WriteFile(m_hComm, &msg, 1, &iBytesWritten, NULL)) {
		if (iBytesWritten == 1) {
			return true;
		}
	}
	return false;
}

bool COvsSerial::WriteByte(const char* msg, int size)
{
	DWORD iBytesWritten = 0;
	if (m_hComm != NULL) {
		if (WriteFile(m_hComm, msg, size, &iBytesWritten, NULL)) {
			if (iBytesWritten == size) {
				return true;
			}
		}
	}
	return false;
}

bool COvsSerial::ReadByte(BYTE &msg)
{
	BYTE rx;
	msg = 0;
	DWORD dwBytesTransferred = 0;
	if (m_hComm != NULL) {
		if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {
			if (dwBytesTransferred == 1) {
				msg = rx;
				return true;
			}
		}
	}
	return false;
}

bool COvsSerial::ReadByte(BYTE* &msg, UINT size)
{
	DWORD dwBytesTransferred = 0;
	if (m_hComm != NULL) {
		if (ReadFile(m_hComm, msg, size, &dwBytesTransferred, 0)) {
			if (dwBytesTransferred == size) {
				return true;
			}
		}
	}
	return false;
}

int COvsSerial::openSerial()
{
	if (OpenDevice("067B", "2303", 115200)) {
		m_bOpenComm = true;
		return 0; //basic
	}
	else if (OpenDevice("2341", "8036", 115200)) {
		m_bOpenComm = true;
		return 1; //lattepanda
	}
	else {
		m_bOpenComm = false;
		return -1; //license
	}
}

bool COvsSerial::isOpenSerial()
{
	return m_bOpenComm;
}

void COvsSerial::closeSerial()
{
	CloseHandle(m_hComm);
}

void COvsSerial::ledControl(const char* color)
{
	std::string strCmd = "#LED@" + std::string(color) + "$";
	WriteByte(strCmd.c_str(), (int)strCmd.length());
}

void COvsSerial::beepControl(const char* mode)
{
	std::string strCmd = "#BUZZER@" + std::string(mode) + "$";
	WriteByte(strCmd.c_str(), (int)strCmd.length());
}
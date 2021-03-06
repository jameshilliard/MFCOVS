#include "SerialPort.h"

SerialPort::SerialPort()
	:b_comm_state(false)
{
	h_comm = INVALID_HANDLE_VALUE;
}

SerialPort::~SerialPort()
{
	b_comm_state = false;
	if (h_comm !=INVALID_HANDLE_VALUE)
	{
		CloseHandle(h_comm);
		h_comm = INVALID_HANDLE_VALUE;
	}
}

void SerialPort::CloseComm()
{
	if (h_comm == INVALID_HANDLE_VALUE)
	{
		return;
	}
	CloseHandle(h_comm);
}

bool SerialPort::WDK_WhoAllVidPid(string& ssin,string& serial_num)
{
	HIDD_VIDPID* pVidPid = new HIDD_VIDPID[9];
	int iCapacity = 32;
	GUID* SetupClassGuid = NULL;
	GUID* InterfaceClassGuid = NULL;

	// 根据设备安装类GUID创建空的设备信息集合
	HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(SetupClassGuid, NULL);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// 根据设备安装类GUID获取设备信息集合
	HDEVINFO hDevInfo;

	if (InterfaceClassGuid == NULL)
	{
		hDevInfo = SetupDiGetClassDevsEx(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE | DIGCF_PRESENT, DeviceInfoSet, NULL, NULL);
	}
	else
	{
		hDevInfo = SetupDiGetClassDevsEx(InterfaceClassGuid, (PCWSTR) & "SCSI", NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT, DeviceInfoSet, NULL, NULL);
	}

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// 存储设备实例ID	
	TCHAR DeviceInstanceId[DeviceInstanceIdSize];

	// 存储设备信息数据
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// 获取设备信息数据
	DWORD DeviceIndex = 0;
	bool k = true;
	while (SetupDiEnumDeviceInfo(hDevInfo, DeviceIndex++, &DeviceInfoData))
	{
		// 获取设备实例ID
		if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, DeviceInstanceId, DeviceInstanceIdSize, NULL) && k)
		{
			// 从设备实例ID中提取VID和PID
			TCHAR* pVidIndex = _tcsstr(DeviceInstanceId, TEXT("VID_"));
			if (pVidIndex == NULL)
			{
				continue;
			}

			TCHAR* pPidIndex = _tcsstr(pVidIndex + 4, TEXT("PID_"));
			if (pPidIndex == NULL)
			{
				continue;
			}

			USHORT VendorID = (USHORT)_tcstoul(pVidIndex + 4, NULL, 16);
			USHORT ProductID = (USHORT)_tcstoul(pPidIndex + 4, NULL, 16);

			//输出串口号

			TCHAR fname[56] = { 0 };

			SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData,
				SPDRP_HARDWAREID,
				0, (PBYTE)fname,
				sizeof(fname),
				NULL);

			//判断相应VID号的串口

			//字符串方法识别FTDI
			string str1(57, '0');
			string s1 = ssin;
			for (int i = 0; i < 56; i++)
			{
				str1[i] = (char)fname[i];
			}


			if (str1.find(s1) == string::npos) continue;



			//清空fname
			for (int i = 0; i < 56; i++)
			{
				fname[i] = 0;
			}

			//	输出串口号
			SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData,
				SPDRP_FRIENDLYNAME,
				0, (PBYTE)fname,
				sizeof(fname),
				NULL);

			string str2(57, '0');
			string s2 = "COM";
			for (int i = 0; i < 56; i++)
			{
				str2[i] = (char)fname[i];
			}
			if (str2.find(s2) != string::npos)
			{
				int num = str2.find(s2);
				serial_num = str2.substr(num, 4);
			}
		}
	}
	return true;
}

bool SerialPort::OpenComm(string& comm_name_str)
{
	h_comm = CreateFile(CString(comm_name_str.c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	// 判断打开com是否成功
	if (h_comm == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool SerialPort::ReadData(BYTE& msg)
{
	BYTE rx;
	msg = 0;
	DWORD dwBytesTransferred = 0;
	if (h_comm != NULL)
	{
		if (ReadFile(h_comm, &rx, 1, &dwBytesTransferred, 0)) 
		{
			if (dwBytesTransferred == 1)
			{
				msg = rx;
				return true;
			}
		}
	}

	return false;
}

bool SerialPort::ReadData(BYTE*& msg, UINT size)
{
	DWORD dwBytesTransferred = 0;
	if (h_comm != NULL)
	{
		if (ReadFile(h_comm, msg, size, &dwBytesTransferred, 0))
		{
			if (dwBytesTransferred == size)
			{
				return true;
			}
		}
	}
	return false;
}

bool SerialPort::WriteData(BYTE& msg)
{
	DWORD iBytesWritten = 0;
	if (WriteFile(h_comm, &msg, 1, &iBytesWritten, NULL)) 
	{
		if (iBytesWritten == 1) 
		{
			return true;
		}
	}
	return false;
}

bool SerialPort::WriteData(char* msg, int size)
{
	DWORD iBytesWritten = 0;
	if (h_comm != NULL) 
	{
		if (WriteFile(h_comm, msg, size, &iBytesWritten, NULL))
		{
			if (iBytesWritten == size)
			{
				return true;
			}
		}
	}
	return false;
}

bool SerialPort::CleanComm()
{
	return PurgeComm(h_comm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
}

bool SerialPort::SetCommunicationTimeouts(COMMTIMEOUTS& in_commtimeouts)
{
	if ((b_comm_ready = GetCommTimeouts(h_comm, &m_commtimeouts)) == 0)
	{
		CloseHandle(h_comm);
		return false;
	}
	m_commtimeouts = in_commtimeouts;
	//m_commtimeouts.ReadIntervalTimeout = read_interval_timeout;
	//m_commtimeouts.ReadTotalTimeoutMultiplier = read_total_timeout_multiplier;
	//m_commtimeouts.ReadTotalTimeoutConstant = read_total_timeout_constant;
	//m_commtimeouts.WriteTotalTimeoutMultiplier = write_total_timeout_multiplier;
	//m_commtimeouts.WriteTotalTimeoutConstant = write_total_timeout_constant;

	if ((b_comm_ready = SetCommTimeouts(h_comm, &m_commtimeouts)) == 0)
	{
		CloseHandle(h_comm);
		return false;
	}
	return true;

}

bool SerialPort::ConfigureComm(DWORD& baud_rate, BYTE& byte_size, BYTE& parity, BYTE& stop_bits)
{
	if ((b_comm_ready = GetCommState(h_comm, &m_dcb)) == 0)
	{
		CloseHandle(h_comm);
		return false;
	}

	m_dcb.BaudRate = baud_rate;
	m_dcb.ByteSize = byte_size;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.fBinary = true;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fParity = false;
	m_dcb.fOutX = false;
	m_dcb.fInX = false;
	m_dcb.fNull = false;
	m_dcb.fAbortOnError = true;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxDsrFlow = false;
	m_dcb.fDtrControl = DTR_CONTROL_ENABLE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fRtsControl = RTS_CONTROL_ENABLE;

	if ((b_comm_ready = SetCommState(h_comm, &m_dcb)) == 0)
	{
		CloseHandle(h_comm);
		return false;
	}

	return true;
}

void SerialPort::delay_msec(int msec)
{
	clock_t now = clock();
	while (clock() - now < msec);
}

/*bool SerialPort::ModfiyColor(string in_color)
{
	string strCmd = "#LED@ON," + in_color + "$";
	WriteData(strCmd.c_str(), (int)strCmd.length());
}

bool SerialPort::LedHeartBeat()
{
	std::string strCmd = "#LED@HEARTBEAT$";
	WriteData(strCmd.c_str(), (int)strCmd.length());
}*/

bool SerialPort::SerialPortInit()
{
	string name_comm_str = "";
	string vid_pid = "VID_1A86&PID_7523";
	if (WDK_WhoAllVidPid(vid_pid, name_comm_str))
	{
		if (OpenComm(name_comm_str))
		{
			b_comm_state = true;
			return true;
		}
	}
	return false;
}
//
//bool SerialPort::BackMechanicalZero()
//{
//	char in_char[] = "55aa0b0a204e000000c3";
//	char out_char[21];
//	out_char[20] = '\0';
//	int hex_length = 10;
//	to_hex(in_char, hex_length, out_char);
//	if (b_comm_state)
//	{
//		/*定义一个COMMTIMEOUTES结构并初始化设置参数*/
//		COMMTIMEOUTS ctTimSet;
//		ctTimSet.ReadIntervalTimeout = 1;
//		ctTimSet.ReadTotalTimeoutMultiplier = 100;
//		ctTimSet.ReadTotalTimeoutConstant = 0;
//		ctTimSet.WriteTotalTimeoutMultiplier = 0;
//		ctTimSet.WriteTotalTimeoutConstant = 0;
//
//		if (SetCommunicationTimeouts(ctTimSet))
//		{
//			if (!SetupComm(h_comm, 1024, 1024))
//			{
//				return false;
//			}
//			DWORD BaudRate = CBR_115200;
//			BYTE ByteSize = 8;
//			DWORD fParity = 0;
//			BYTE Parity = 'N';
//			BYTE StopBits = 1;
//			if (ConfigureComm(BaudRate, ByteSize, Parity, StopBits))
//			{
//				WriteData(out_char, hex_length);
//			}
//		}
//	}
//	PurgeComm(h_comm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
//	return true;
//}

void SerialPort::to_hex(char* in_char, int char_length, char* out_char)
{
	while (char_length--)
	{
		*out_char = (*in_char & 0x40 ? *in_char + 9 : *in_char) << 4;
		++in_char;
		*out_char |= (*in_char & 0x40 ? *in_char + 9 : *in_char) & 0xF;
		++in_char;
		++out_char;
	}
}

std::string SerialPort::TCHARToString(TCHAR* STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);

	char* chRtn = new char[iLen * sizeof(char)];

	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);

	std::string str(chRtn);

	return str;
}

char* SerialPort::TCHARTochar(TCHAR* STR)
{
	//返回字符串的长度

	int size = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, FALSE);

	//申请一个多字节的字符串变量

	char* str = new char[sizeof(char) * size];

	//将STR转成str

	WideCharToMultiByte(CP_ACP, 0, STR, -1, str, size, NULL, FALSE);

	return str;
}

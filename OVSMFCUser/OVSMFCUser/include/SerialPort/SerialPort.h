/***********************************************
文件名：Comm.h
类型：C++头文件
功能：实现对串口操作的封装
时间：2018.7.30
**********************************************/
#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <atlstr.h>
#include <SetupAPI.h>
#include <time.h>
#define DeviceInstanceIdSize 256

#pragma comment(lib, "Setupapi.lib")
using namespace std;
typedef struct _HIDD_VIDPID
{
	USHORT	VendorID;
	USHORT	ProductID;
} HIDD_VIDPID;

class SerialPort
{
public:
	SerialPort();
	~SerialPort();

private:
	// 串口句柄
	HANDLE h_comm;
	// 串口状态信息
	DCB m_dcb;
	COMMTIMEOUTS m_commtimeouts;
//VERLAPPED m_hReadEvent;
	// 查找串口
	bool WDK_WhoAllVidPid(string& ssin, string& serial_num);
	// 打开串口
	bool OpenComm(string& comm_name_str);

	// 清理缓存区
	bool CleanComm();

	/********************************
	* @brief:对串口超时参数进行设置
	* @param: DWORD& read_interval_timeout           读间隔超时时间
	* @param: DWORD& read_total_timeout_multiplier   读时间系数
	* @param: DWORD& read_total_timeout_constant     读时间常量
	* @param: DWORD& write_total_timeout_multiplier  写时间系数
	* @param: DWORD& write_total_timeout_constant    写时间常量
	*
	* @return: bool
	*********************************/
	bool SetCommunicationTimeouts(COMMTIMEOUTS& in_commtimeouts);

	/********************************
	* @brief:对串口超时参数进行设置
	* @param: DWORD& baud_rate  波特率
	* @param: BYTE& byte_size   通信字节位数
	* @param: BYTE& parity      指定奇偶校验位
	* @param: BYTE& stop_bits   指定停止位的位数
	*
	* @return: bool
	*********************************/
	bool ConfigureComm(DWORD baud_rate);

	// 延时
	void delay_msec(int msec);

	void to_hex(char* in_char, int char_length, char* out_char);
	string TCHARToString(TCHAR* STR);
	char* TCHARTochar(TCHAR* STR);

public:

	// 初始化串口
	bool SerialPortInit(string vid, string pid);

	// 关闭串口
	void CloseComm();

	// 读取数据
	bool ReadData(BYTE& msg);
	bool ReadData(const char* msg, int size);

	int ReadTest(LPBYTE lpBytes, DWORD dwLen);
	// 写入数据
	bool WriteData(BYTE& msg);
	bool WriteData(const char* msg, int size);

	// 回到机械0
	bool BackMechanicalZero();

	//正向运行
	bool MotorIncreaseMillimeter(int val);
	
	//反向运行
	bool MotorDecreaseMillimeter(int val);
	
	// 到10
	bool MotorToTen();

	void ledColorControl(const char* color);
	void ledClose();
	void ledHeartBeat();

	void GetTemperature(float& in_temp);

	// 串口状态
	bool b_comm_state;

	// 串口是否打开
	bool b_comm_ready;

	// 点击状态
	bool b_motor_state;

};
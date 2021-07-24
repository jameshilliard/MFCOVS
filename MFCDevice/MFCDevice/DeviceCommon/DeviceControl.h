#include <windows.h>
#include <string>
#include <memory>
#include <iostream>
#include <ostream>
#include <sstream>
#include <tchar.h>
#include <iomanip>
#include <algorithm>
#include <SonixCamera.h>

using namespace std;


class DeviceControl
{ 
public:
	DeviceControl();
	~DeviceControl();
	bool m_device_status = false;
	bool DeviceInit();

	HRESULT OpenDevice();
	HRESULT CloseDevice();

	HRESULT Set_CompressionRatio(string val);
	HRESULT	Get_CompressionRatio(string& val);

	HRESULT Set_Exposure(string val);
	HRESULT Get_Exposure(string& val);

	HRESULT Set_Gain(string val);
	HRESULT Get_Gain(string& val);

	HRESULT Set_LEDCurrent(string val);
	HRESULT Get_LEDCurrent(string& val);

	HRESULT Set_BL(string val);
	HRESULT Get_BL(string& val);

	HRESULT Get_Distance(string& val);
	HRESULT Enable_Distance_Function(bool enable);
	HRESULT Enable_DSP_Function(bool enable);

private:

	HRESULT AsicRegisterWrite(unsigned short addr, string val, int len);
	HRESULT AsicRegisterRead(unsigned short addr, string& val, int len);


	HRESULT COMS_Gain_High_1_bit_Write(string val);
	HRESULT COMS_Gain_Low_8_bit_Write(string val);
	HRESULT COMS_Gain_Low_8_bit_Read(string& val);

	HRESULT COMS_Exposure_High_8_bit_Write(string val);
	HRESULT COMS_Exposure_Middle_8_bit_Write(string val);
	HRESULT COMS_Exposure_Middle_8_bit_Read(string& val);
	HRESULT COMS_Exposure_Low_8_bit_Write(string val);

	void Dec2Hex(int x, string& in_str);
	void Dec2HexK(int x, string& in_str);
	void Dec2HexP(int x, string& in_str);

private:
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
	unsigned short DSP_TO_DISTANCE_FUN_ADDR = 0x0C8F;

	unsigned short DSP_TO_COMS_ADDR_H = 0x0C90;
	unsigned short DSP_TO_COMS_ADDR_L = 0x0C91;
	unsigned short DSP_TO_COMS_ADDR_D = 0x0C92;
	unsigned short DSP_TO_COMS_ADDR_T = 0x0C93;

	unsigned short DSP_TO_LED_ADDR = 0x0C94;
};
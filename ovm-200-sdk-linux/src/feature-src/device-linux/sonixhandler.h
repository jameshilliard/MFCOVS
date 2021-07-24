#ifndef SONIXHANDLER_H
#define SONIXHANDLER_H

#include <string>
#include <stdint.h>
#include <map>
#include <vector>

#include "util.h"

using namespace std;

class SonixHandler
{
public:
    explicit SonixHandler();
    ~SonixHandler();
    static SonixHandler *instance();

    bool init();

    bool OpenDevice();
    bool CloseDevice();

    bool Set_Exposure(string val);
    bool Get_Exposure(string& val);

    bool Set_LEDCurrent(string val);
    bool Get_LEDCurrent(string& val);
    //
    bool Set_BLOpen(const bool flag);
    bool Get_BL(string& val);

    bool Get_Distance(string& val);
//flash
    bool dataMapWriteToFlash();
    bool dataMapReadFromFlash();

    bool setDevName(const string& strName);
    bool getDevName(string& strName);

    bool setDevSN(const string& strSN);
    bool getDevSN(string& strSN);

    bool setDevFireWareVer(const string& s);
    bool getDevFireWareVer(string& s);

    bool setDevHardWareVer(const string& strHWVer);
    bool getDevHardWareVer(string& strHWVer);

    bool setDevSoftWareVer(const string& strSWVer);
    bool getDevSoftWareVer(string& strSWVer);
private:
    //
    bool DeviceInit();
    //
    bool isFlashInit();
    void flashInit();

    bool flashWriter(const string &str);
    bool flashReader(const int byteLength,string &str);
    string funcAccumulate(const vector<string>& vtrs,
                          const string& sSeperator);
    vector<string> funcSplit(const string& s, const string& seperator);

    map<string,string> _mapFlashData;
    vector<string> _vtrFixFlashParamName;
    int _iFlashStatus;

    //
    bool Set_Gain(string val);
    bool Get_Gain(string& val);

    bool Set_CompressionRatio(string val);
    bool Get_CompressionRatio(string& val);

    bool Enable_Distance_Function(bool enable);
    bool Enable_DSP_Function(bool enable);

    int hex2int(string &s);

private:
    bool AsicRegisterWrite(unsigned short addr, string val, int len);
    bool AsicRegisterRead(unsigned short addr, string& val, int len);

    bool COMS_Gain_High_1_bit_Write(string val);
    bool COMS_Gain_Low_8_bit_Write(string val);
    bool COMS_Gain_Low_8_bit_Read(string& val);

    bool COMS_Exposure_High_8_bit_Write(string val);
    bool COMS_Exposure_Middle_8_bit_Write(string val);
    bool COMS_Exposure_Middle_8_bit_Read(string& val);
    bool COMS_Exposure_Low_8_bit_Write(string val);

    bool COMS_BL_8_bit_Write(string val);

    void Dec2Hex(int x, string& in_str);
    void Dec2HexK(int x, string& in_str);
    void Dec2HexP(int x, string& in_str);
private:
    const unsigned short COMS_IMAGE_STREAM_ADDR = 0x0C83;

    const unsigned short COMS_COMPRESSIONRATIO_ADDR = 0x0C84;

    const unsigned short COMS_EXP_ADDR_H8 = 0x3500;
    const unsigned short COMS_EXP_ADDR_M8 = 0x3501;
    const unsigned short COMS_EXP_ADDR_L8 = 0x3502;

    const unsigned short COMS_GAIN_ADDR_H1 = 0x350A;
    const unsigned short COMS_GAIN_ADDR_L8 = 0x350B;

    const unsigned short COMS_BLACK_LEVEL_ADDR = 0x4007;

    const unsigned short DSP_FUNCTION_INTERRUPT_ADDR = 0x1003;

    const unsigned short DSP_TO_DISTANCE_ADDR = 0x0C8A;
    const unsigned short DSP_TO_DISTANCE_FUN_ADDR = 0x0C8F;

    const unsigned short DSP_TO_COMS_ADDR_H = 0x0C90;
    const unsigned short DSP_TO_COMS_ADDR_L = 0x0C91;
    const unsigned short DSP_TO_COMS_ADDR_D = 0x0C92;
    const unsigned short DSP_TO_COMS_ADDR_T = 0x0C93;

    const unsigned short DSP_TO_LED_ADDR = 0x0C94;
};

#endif // SONIXHANDLER_H

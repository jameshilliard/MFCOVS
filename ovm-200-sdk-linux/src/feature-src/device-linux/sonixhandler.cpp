#include "sonixhandler.h"
#include "linux/videodev2.h"
#include "opencv2/opencv.hpp"
#include "logger.h"
#include "stdio.h"
#include "global.h"
#include "ovm_200_feature_dev.h"

extern "C"{
#include "SonixCamera.h"
}

#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

using namespace std;

typedef unsigned char uchar;
typedef pair<string,string> pairss;

const string conStrFlashStatus("flashStatus");
const string conStrFlashDevName("devName");
const string conStrFlashSerialNum("SN");
const string conStrFlashFirmWareVersion("firmWareVersion");
const string conStrFlashHardWareVersion("hardWareVersion");
const string conStrFlashSoftWareVersion("softWareVersion");

const string conStrFlashDataSeperator = "--;";

const int conFlashAddr = 0x1F000;

const int conLengthOfFlashData   = 1000;


enum E_FlashStatus{
    e_inited = 0x01,
};

SonixHandler::SonixHandler():
    _iFlashStatus(0)
{
    //init param
    _vtrFixFlashParamName.push_back(conStrFlashStatus);
    _vtrFixFlashParamName.push_back(conStrFlashDevName);
    _vtrFixFlashParamName.push_back(conStrFlashSerialNum);
    _vtrFixFlashParamName.push_back(conStrFlashFirmWareVersion);
    _vtrFixFlashParamName.push_back(conStrFlashHardWareVersion);
    _vtrFixFlashParamName.push_back(conStrFlashSoftWareVersion);

    for(auto item:_vtrFixFlashParamName){
        _mapFlashData[item] = string("");
    }
}

SonixHandler::~SonixHandler()
{
    CloseDevice();

    logde<<("SonixCam_UnInit:%d\n",SonixCam_UnInit());
}

SonixHandler *SonixHandler::instance()
{
    static SonixHandler ins;
    return &ins;
}

bool SonixHandler::init()
{
    //init dev
    if(false == DeviceInit()){
        return false;
    }
    //
    bool bRe = false;
    for(int i = 0; i < g_iFlashReReadTimes; i++){
        bRe = dataMapReadFromFlash();
        if(bRe){
            break;
        }else{
            flashInit();
        }
    }

    if(false == bRe){
        return false;
    }

    //
    if(isFlashInit()){
        logde<<"sonix handler,flash inited check,succ.";
    }else{
        logde<<"sonix handler,flash inited check,faild.";
        flashInit();
    }

    return true;
}

bool SonixHandler::OpenDevice()
{
    bool ret = false;
    //ret = Enable_Distance_Function(true);
    //if (ret != true)
    //{
    //	return ret;
    //}
    ret = AsicRegisterWrite(COMS_IMAGE_STREAM_ADDR, "01", 1);
    //if (ret != true)
    //{
    //	return ret;
    //}
    return ret;
}

bool SonixHandler::CloseDevice()
{

    bool ret = false;
    //ret = Enable_Distance_Function(false);
    //if (ret != true)
    //{
    //	return ret;
    //}
    ret = AsicRegisterWrite(COMS_IMAGE_STREAM_ADDR, "00", 1);
    if (ret != true)
    {
        logde<<("CloseDevice,error\n");
        //        return ret;
    }else{
        logde<<("CloseDevice,succ\n");
    }

    string s;
    ret = AsicRegisterRead(COMS_IMAGE_STREAM_ADDR, s, 1);

    return ret;
}


bool SonixHandler::Set_CompressionRatio(string val)
{
    bool ret = false;
    ret = AsicRegisterWrite(COMS_COMPRESSIONRATIO_ADDR, val, 1);
    return ret;
}

bool SonixHandler::Get_CompressionRatio(string& val)
{
    bool ret = false;
    ret = AsicRegisterRead(COMS_COMPRESSIONRATIO_ADDR, val, 1);
    return ret;
}


bool SonixHandler::Set_Exposure(string val) //ֻ������8λ
{
    bool ret = false;
    if(stoi(val) <= 0){
        return ret;
    }
    if(stoi(val) >= 60){
        return ret;
    }

    ret = Enable_Distance_Function(false);
    if (ret != true)
    {
        return ret;
    }
    ret = COMS_Exposure_Middle_8_bit_Write(val);
    if (ret != true)
    {
        return ret;
    }
    ret = Enable_Distance_Function(true);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::Get_Exposure(string& val)
{
    COMS_Exposure_Middle_8_bit_Read(val);

    return true;
}

bool SonixHandler::Set_Gain(string val)//ֻ���ڵ�8λ
{
    return COMS_Gain_Low_8_bit_Write(val);
}

bool SonixHandler::Get_Gain(string& val)
{
    return COMS_Gain_Low_8_bit_Read(val);
}

bool SonixHandler::Set_LEDCurrent(string val)
{
    bool ret = false;
    ret = Enable_Distance_Function(false);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_LED_ADDR, val, 1);

    ret = Enable_Distance_Function(true);
    if (ret != true)
    {
        return ret;
    }

    return ret;
}

bool SonixHandler::Get_LEDCurrent(string& val)
{
    bool ret = false;
    ret = AsicRegisterRead(DSP_TO_LED_ADDR, val, 1);

    return ret;
}

bool SonixHandler::Set_BLOpen(const bool flag)
{
    bool ret = false;
    ret = Enable_Distance_Function(false);
    if (ret != true)
    {
        return ret;
    }

    if(flag){
        ret = COMS_BL_8_bit_Write(string("00"));
    }else{
        ret = COMS_BL_8_bit_Write(string("16"));
    }

    ret = Enable_Distance_Function(true);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::Get_BL(string& val)
{
    bool ret = false;
    ret = AsicRegisterRead(COMS_BLACK_LEVEL_ADDR, val, 1);
    return ret;

}

bool SonixHandler::Enable_DSP_Function(bool enable)
{
    bool ret = false;
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

bool SonixHandler::Enable_Distance_Function(bool enable)
{
    bool ret = false;
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


bool SonixHandler::Get_Distance(string& val)
{
    bool ret = false;
    //    ret = Enable_Distance_Function(false);
    //    if (ret != true)
    //    {
    //        return ret;
    //    }

    ret = AsicRegisterRead(DSP_TO_DISTANCE_ADDR, val, 1);

    //if (ret != true)
    //{
    //	return ret;
    //}
    //ret = Enable_Distance_Function(true);
    //if (ret != true)
    //{
    //	return ret;
    //}
    return ret;
}

bool SonixHandler::setDevName(const string &s)
{
    _mapFlashData[conStrFlashDevName] = s;
    return true;
}

bool SonixHandler::getDevName(string &s)
{
    s = _mapFlashData[conStrFlashDevName];
    return true;
}

bool SonixHandler::setDevSN(const string &s)
{
    _mapFlashData[conStrFlashSerialNum] = s;
    return true;
}

bool SonixHandler::getDevSN(string &s)
{
    s =  _mapFlashData[conStrFlashSerialNum];
    return true;
}

bool SonixHandler::setDevFireWareVer(const string &s)
{
    _mapFlashData[conStrFlashFirmWareVersion] = s;
    return true;
}

bool SonixHandler::getDevFireWareVer(string &s)
{
    s =  _mapFlashData[conStrFlashFirmWareVersion];
    return true;
}

bool SonixHandler::setDevHardWareVer(const string &s)
{
    _mapFlashData[conStrFlashHardWareVersion] = s;
    return true;
}

bool SonixHandler::getDevHardWareVer(string &s)
{
    s = _mapFlashData[conStrFlashHardWareVersion];
    return true;
}

bool SonixHandler::setDevSoftWareVer(const string &s)
{
    _mapFlashData[conStrFlashSoftWareVersion] = s;
    return true;
}

bool SonixHandler::getDevSoftWareVer(string &s)
{
    s = _mapFlashData[conStrFlashSoftWareVersion];
    return true;
}

bool SonixHandler::isFlashInit()
{
    //
    string sFlashSutatus = _mapFlashData[conStrFlashStatus].data();
    if(atoi(sFlashSutatus.data()) ==
            E_FlashStatus::e_inited){
        return true;
    }else{
        return false;
    }
}

void SonixHandler::flashInit()
{
    //
    _mapFlashData[conStrFlashStatus]
            = to_string(E_FlashStatus::e_inited);

    _mapFlashData[conStrFlashDevName]  = string("devName-default");
    _mapFlashData[conStrFlashSerialNum]  = string("sn-default");
    _mapFlashData[conStrFlashFirmWareVersion]  = string("fwv-default");
    _mapFlashData[conStrFlashHardWareVersion]  = string("hwv-default");
    _mapFlashData[conStrFlashSoftWareVersion]  = string("swv-default");
    //
    dataMapWriteToFlash();
}

string SonixHandler::funcAccumulate(const vector<string>& vtrs,
                                    const string& sSeperator){
    string s =  accumulate(
                vtrs.begin(),vtrs.end(),string(""),
                [&](const string &s1,const string &s2){
        if(s1.empty()){
            return s2;
        }
        return s1 + sSeperator + s2;
    });
    return (s+conStrFlashDataSeperator);
}

bool SonixHandler::dataMapWriteToFlash()
{
    for(auto &item:_mapFlashData){
        logde<<"map flash write data:"<<item.first<<"/"<<item.second;
    }

    vector<string> vtrsData;
#if 0
    vtrsData.push_back(_mapFlashData[conStrFlashStatus]);
    vtrsData.push_back(_mapFlashData[conStrFlashDevName]);
    vtrsData.push_back(_mapFlashData[conStrFlashSerialNum]);
    vtrsData.push_back(_mapFlashData[conStrFlashFirmWareVersion]);
    vtrsData.push_back(_mapFlashData[conStrFlashHardWareVersion]);
    vtrsData.push_back(_mapFlashData[conStrFlashSoftWareVersion]);
#endif
    for(auto &item:_vtrFixFlashParamName){
        vtrsData.push_back(_mapFlashData[item]);
    }

    string sData = funcAccumulate(vtrsData,conStrFlashDataSeperator);

    logde<<"data map write to flash:"
        <<sData;

    return flashWriter(sData);

#if 0
    for(auto &item:_mapFlash)    {
        if(item.first == conStrFlashDevName){

        }
    }
#endif

}

bool SonixHandler::dataMapReadFromFlash()
{
    string sData;
    bool bRe = flashReader(conLengthOfFlashData,sData);
    if(false == bRe){
        logde<<"flash reader failed.";
        return false;
    }

    logde<<"read from flash:"<<sData;

    vector<string> vtrs;
    vtrs = funcSplit(sData,conStrFlashDataSeperator);

    logde<<"vtrs size:"<<vtrs.size();

    if(vtrs.size() == 0){
        logde<<"data map read from falsh,vtrs size is 0.";
        return false;
    }

    if(vtrs.size() < _vtrFixFlashParamName.size()){
        logde<<"data map read from falsh,vtrs size is small.";
        return false;
    }

    for(int i = 0; i < _vtrFixFlashParamName.size(); i++){
        _mapFlashData[_vtrFixFlashParamName[i]] = vtrs[i];
    }

    logde<<"sdk feature,data map read from flash."   ;
    for(auto &item:_mapFlashData){
        logde<<item.first<<"/"<<item.second;
    }
    return true;
}

vector<string> SonixHandler::funcSplit(const string &s, const string &seperator){
    vector<string> result;
    std::size_t posBegin = 0;
    std::size_t posSeperator = s.find(seperator);

    while (posSeperator != s.npos) {
        result.push_back(s.substr(posBegin, posSeperator - posBegin));//
        posBegin = posSeperator + seperator.size(); // 分隔符的下一个元素
        posSeperator = s.find(seperator, posBegin);
    }
#if 0
    if (posBegin != s.length()) // 指向最后一个元素，加进来
        result.push_back(s.substr(posBegin));
#endif
    return result;
}

bool SonixHandler::flashWriter(const string &s){
    if(s.empty()){
        return false;
    }

    if(s.size() > conLengthOfFlashData){
        return false;
    }

    uint8_t tmpData[conLengthOfFlashData] = {'\0'};
    memcpy(tmpData,s.data(),s.length());

    //write
    return SonixCam_SerialFlashSectorCustomWrite(
                conFlashAddr, tmpData, s.length(), SFT_MXIC);
}

bool SonixHandler::flashReader(
        const int length,string &str)
{
    str.clear();

    if(length <= 0){
        return false;
    }

    uchar data[conLengthOfFlashData] = {'\0'};

    bool bRe =  SonixCam_SerialFlashCustomRead(
                conFlashAddr,data,conLengthOfFlashData);

    if(bRe){
        str = string((char*)data,strlen((char*)data));
        return true;
    }else{
        return false;
    }
}

bool SonixHandler::DeviceInit()
{
    bool ret = false;

    ret = SonixCam_Init(g_moduleVideoNum);

    if (false == ret)
    {
        logde<<"error,sonix cam init failed.";
        return false;
    }

    ret = OpenDevice();

    //Sleep(2*1000);
    /*ret = SonixCam_SetVideoFormat(NULL, VCS_Yuy2);
    if (ret != true)
    {
        logde<<("ERROR: SonixCam_SetVideoFormat() false!\n");
        return false;
    }*/
    ////�ȴ�ͼ����
    //if (ret != true)
    //{
    //	logde<<("ERROR: OpenDevice() - Open Device false!\n");
    //	return false;
    //}
    //��ʼ���ع�

    ret = COMS_Exposure_High_8_bit_Write("00");

    if (ret != true)
    {
        logde<<("ERROR: COMS_Exposure_High_8_bit_Write() - COMS Exposure High 8bit Write false!\n");
        return false;
    }

    ret = COMS_Exposure_Middle_8_bit_Write("28");

    if (ret != true)
    {
        logde<<("ERROR: COMS_Exposure_Middle_8_bit_Write() - COMS Exposure Middle 8bit Write false!\n");
        return false;
    }
    ret = COMS_Exposure_Low_8_bit_Write("00");

    if (ret != true)
    {
        logde<<("ERROR: COMS_Exposure_Low_8_bit_Write() - COMS Exposure Low 8bit Write false!\n");
        return false;
    }

    //��ʼ������
    ret = COMS_Gain_High_1_bit_Write("00");
    if (ret != true)
    {
        logde<<("ERROR: COMS_Gain_High_1_bit_Write() - COMS Gain High 1bit Write false!\n");
        return false;
    }
    ret = COMS_Gain_Low_8_bit_Write("00");
    if (ret != true)
    {
        logde<<("ERROR: COMS_Gain_Low_8_bit_Write() - COMS Gain Low 8bit Write false!\n");
        return false;
    }

    //��ʼ��ѹ����
    ret = Set_CompressionRatio("05");
    if (ret != true)
    {
        logde<<("ERROR: Set_CompressionRatio() - COMS Compression Ratio Set false!\n");
        return false;
    }

    return ret;
}

bool SonixHandler::AsicRegisterWrite(
        unsigned short addr,
        std::string val,
        int len)
{
    bool ret = false;
    BYTE* pData = new BYTE[len];
    memset(pData, 0xff, len);
    LONG sIndex = -1;
    string sTemp = val;
    LONG srcLength = val.length();
    BYTE add = 0;
    LONG i = 0;
    char temp[3] = { '\0' };
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

bool SonixHandler::AsicRegisterRead(
        unsigned short addr,
        std::string &val,
        int len)
{
    bool ret = false;
    //    uchar pData[len] = {0xff};
    uchar pData[len] = {0};
    memset(pData, 0xff, len);

    ret = SonixCam_AsicRegisterRead(addr, pData, len);
    //
    val = to_string((int)*pData);

    //    for (int i = 0; i < len; i++)
    //    {
    //        if (((i % 16) == 0) && (i != 0)){
    //            Dec2HexK(pData[i], val);
    //        }
    //        else{
    //            Dec2HexP(pData[i], val);
    //        }
    //    }

    return ret;
}

bool SonixHandler::COMS_Gain_High_1_bit_Write(std::string val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_GAIN_ADDR_H1 >> 8;
    Addr_L = COMS_GAIN_ADDR_H1 & 0xff;
    string Addr_H_Str, Addr_L_Str;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, val, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::COMS_Gain_Low_8_bit_Write(std::string val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_GAIN_ADDR_L8 >> 8;
    Addr_L = COMS_GAIN_ADDR_L8 & 0xff;
    string Addr_H_Str, Addr_L_Str;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, val, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::COMS_Gain_Low_8_bit_Read(std::string &val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_GAIN_ADDR_L8 >> 8;
    Addr_L = COMS_GAIN_ADDR_L8 & 0xff;
    string Addr_H_Str, Addr_L_Str;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "00", 1);
    if (ret != true)
    {
        return ret;
    }

    ret = AsicRegisterRead(DSP_TO_COMS_ADDR_D, val, 1);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::COMS_Exposure_High_8_bit_Write(std::string val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = (COMS_EXP_ADDR_H8 & 0xFF00) >> 8;
    Addr_L = (COMS_EXP_ADDR_H8 & 0x00FF) << 8;
    string Addr_H_Str, Addr_L_Str;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, val, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::COMS_Exposure_Middle_8_bit_Write(std::string val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_EXP_ADDR_M8 >> 8;
    Addr_L = COMS_EXP_ADDR_M8 & 0xff;
    string Addr_H_Str, Addr_L_Str,getData_H,getData_L;
    string tmpHexStr;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);

    string hexValue;
    Dec2Hex(stoi(val),hexValue);

    int ih = 0;
    do{
        if(ih++ > 100){
            break;
        }

        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
        if (ret != true)
        {
            return ret;
        }
        AsicRegisterRead(DSP_TO_COMS_ADDR_H, getData_H, 1);

        tmpHexStr.clear();
        Dec2Hex(stoi(getData_H),tmpHexStr);

    }while (tmpHexStr != Addr_H_Str);

    int il = 0;
    do{
        if(il++ > 100){
            break;
        }

        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
        if (ret != true)
        {
            return ret;
        }

        ret = AsicRegisterRead(DSP_TO_COMS_ADDR_L, getData_L, 1);

        tmpHexStr.clear();
        Dec2Hex(stoi(getData_L),tmpHexStr);

    }while (tmpHexStr != Addr_L_Str);

    int ivalue = 0;
    string getValue;
    do{
        if(ivalue++ >100){
            break;
        }

        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, hexValue, 1);

        if (ret != true)
        {
            return ret;
        }

        ret = AsicRegisterRead(DSP_TO_COMS_ADDR_D, getValue, 1);
        //
        tmpHexStr.clear();
        Dec2Hex(stoi(getValue),tmpHexStr);

        //        logde<<"set exposure:"
        //            <<hexValue<<"|"
        //           <<tmpHexStr
        //          <<"|"<<getValue;

    }while(tmpHexStr != hexValue);

    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
    if (ret != true)
    {
        return ret;
    }

#if 0
    int iflag = 0;
    string getFlag;
    do{
        qDebug()<<"iflag:"<<iflag++;

        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
        if (ret != true)
        {
            return ret;
        }
        ret = AsicRegisterRead(DSP_TO_COMS_ADDR_T, getFlag, 1);
    }while(getFlag != "01");
#endif
    return ret;
}

bool SonixHandler::COMS_Exposure_Middle_8_bit_Read(std::string &val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_EXP_ADDR_M8 >> 8;
    Addr_L = COMS_EXP_ADDR_M8 & 0xff;
    string Addr_H_Str, Addr_L_Str;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "00", 1);
    if (ret != true)
    {
        return ret;
    }

    ret = AsicRegisterRead(DSP_TO_COMS_ADDR_D, val, 1);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::COMS_Exposure_Low_8_bit_Write(std::string val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_EXP_ADDR_L8 >> 8;
    Addr_L = COMS_EXP_ADDR_L8 & 0xff;
    string Addr_H_Str, Addr_L_Str;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, val, 1);
    if (ret != true)
    {
        return ret;
    }
    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
    if (ret != true)
    {
        return ret;
    }
    return ret;
}

bool SonixHandler::COMS_BL_8_bit_Write(string val)
{
    bool ret = false;
    unsigned short Addr_H;
    unsigned short Addr_L;
    Addr_H = COMS_BLACK_LEVEL_ADDR >> 8;
    Addr_L = COMS_BLACK_LEVEL_ADDR & 0xff;
    string Addr_H_Str, Addr_L_Str,getData_H,getData_L;
    string tmpHexStr;
    //Addr_H_Str.Format(_T("%x"), Addr_H);
    //Addr_L_Str.Format(_T("%x"), Addr_L);
    Dec2Hex(Addr_H, Addr_H_Str);
    Dec2Hex(Addr_L, Addr_L_Str);

    string hexValue;
    Dec2Hex(stoi(val),hexValue);
    logde<<"hexvalue:"<<hexValue;

    int ih = 0;
    do{
        ++ih;
        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_H, Addr_H_Str, 1);
        if (ret != true)
        {
            return ret;
        }
        AsicRegisterRead(DSP_TO_COMS_ADDR_H, getData_H, 1);

        //
        tmpHexStr.clear();
        Dec2Hex(stoi(getData_H),tmpHexStr);

    }while (tmpHexStr != Addr_H_Str);

    int il = 0;
    do{
        il++;
        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_L, Addr_L_Str, 1);
        if (ret != true)
        {
            return ret;
        }
        ret = AsicRegisterRead(DSP_TO_COMS_ADDR_L, getData_L, 1);
        //
        tmpHexStr.clear();
        Dec2Hex(stoi(getData_L),tmpHexStr);

    }while (tmpHexStr != Addr_L_Str);

    int ivalue = 0;
    string getValue;
    do{
        ++ivalue;

        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, hexValue, 1);

        if (ret != true)
        {
            return ret;
        }

        ret = AsicRegisterRead(DSP_TO_COMS_ADDR_D, getValue, 1);
        //
        tmpHexStr.clear();
        Dec2Hex(stoi(getValue),tmpHexStr);

    }while(tmpHexStr != hexValue);
    //    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_D, val, 1);

    ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
    if (ret != true)
    {
        return ret;
    }

#if 0
    int iflag = 0;
    string getFlag;
    do{
        qDebug()<<"iflag:"<<iflag++;

        ret = AsicRegisterWrite(DSP_TO_COMS_ADDR_T, "01", 1);
        if (ret != true)
        {
            return ret;
        }
        ret = AsicRegisterRead(DSP_TO_COMS_ADDR_T, getFlag, 1);
    }while(getFlag != "01");
#endif

    return ret;
}

void SonixHandler::Dec2Hex(int x, std::string &in_str)
{
#if 0
    stringstream ss;
    ss << hex << x;
    ss >> in_str;
    transform(in_str.begin(),
              in_str.end(),
              in_str.begin(),
              ::toupper);
#endif

    char buff[200] = {'\0'};
    sprintf(buff,"%02x",x);
    in_str = string(buff);
}

void SonixHandler::Dec2HexK(int x, std::string &in_str)
{
    stringstream ss;
    ss << hex << x << setw(2) << setfill('0');
    ss >> in_str;
    in_str = "\r\n" + in_str;
    transform(in_str.begin(), in_str.end(), in_str.begin(), ::toupper);
}

void SonixHandler::Dec2HexP(int x, std::string &in_str)
{
    stringstream ss;
    ss << hex << x;
    ss >> in_str;
    transform(in_str.begin(), in_str.end(), in_str.begin(), ::toupper);
}

int SonixHandler::hex2int(string &s)
{
    return stoi(s, nullptr, 16);
}


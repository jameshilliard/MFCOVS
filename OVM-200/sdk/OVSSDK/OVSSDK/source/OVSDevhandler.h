#pragma once
#include <string>
#include <iostream>
#include <memory>

#include "OVSDevice.h"
using namespace std;

class DevHandler
{
public:
    DevHandler();
    ~DevHandler();

    // 初始化
    bool Init();
    bool UnInit();

    // 曝光
    bool Set_Exposure(const int val);
    bool Get_Exposure(int& val);

    // LED
    bool Set_LEDCurrent(const int val);
    bool Get_LEDCurrent(int& val);

    // BL
    bool Set_BLOpen(const bool flag);
    bool Get_BL(int& val);

    // 增益
	bool Set_Gain(int val);
	bool Get_Gain(int& val);

    // 图像压缩率
	bool Set_CompressionRatio(int val);
	bool Get_CompressionRatio(int& val);

    // 距离
    bool Get_Distance(int& val);
private:
    string DecToString(int iNum);

    shared_ptr<OVSDevice> p_ovs_control;
};

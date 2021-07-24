#include "DevHandler.h"

DevHandler::DevHandler()
{
    SonixHandler::instance();
}

DevHandler::~DevHandler()
{
    SonixHandler::instance()->CloseDevice();
}

DevHandler *DevHandler::instance()
{
    static DevHandler ins;
    return &ins;
}

bool DevHandler::init()
{
#ifdef WINDOW_SYS       //add for windows
    return OVSDevice::instance()->DeviceInit();
#else
    return SonixHandler::instance()->init();
#endif
}

bool DevHandler::unInit()
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->CloseDevice())
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    return SonixHandler::instance()->CloseDevice();
#endif

}

bool DevHandler::setExposure(const int val)
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->Set_Exposure(val))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    string strVal;
    strVal = decToString(val);
    return SonixHandler::instance()->Set_Exposure(strVal);
#endif

}

bool DevHandler::getExposure(int &val)
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->Get_Exposure(val))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    string strVal;
    strVal = decToString(val);
    if (SonixHandler::instance()->Get_Exposure(strVal)) {
        val = atoi(strVal.c_str());
        return true;
    }
    else {
        val = 0;
        return false;
    }
#endif
}

bool DevHandler::setLEDCurrent(const int val)
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->Set_LEDCurrent(val))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    string strVal;
    strVal = decToString(val);
    return SonixHandler::instance()->Set_LEDCurrent(strVal);
#endif
}

bool DevHandler::getLEDCurrent(int &val)
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->Get_LEDCurrent(val))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    string strVal;
    strVal = decToString(val);
    if (SonixHandler::instance()->Get_LEDCurrent(strVal)) {
        val = atoi(strVal.c_str());
        return true;
    }
    else {
        val = 0;
        return false;
    }
#endif
}

bool DevHandler::setBLOpen(const bool flag)
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->Set_BLOpen(flag))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    return SonixHandler::instance()->Set_BLOpen(flag);
#endif
}

bool DevHandler::getDistance(int &val)
{
#ifdef WINDOW_SYS       //add for windows
    if (S_OK == OVSDevice::instance()->Get_Distance(val))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    string strVal;
    strVal = decToString(val);
    if (SonixHandler::instance()->Get_Distance(strVal)) {
        val = atoi(strVal.c_str());
        return true;
    }
    else {
        val = 0;
        return false;
    }
#endif
}

bool DevHandler::readFromFlash()
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->dataMapReadFromFlash();
#endif
}

bool DevHandler::write2Flash()
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->dataMapWriteToFlash();
#endif
}

bool DevHandler::setDevName(const string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->setDevName(s);
#endif
}

bool DevHandler::getDevName(string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->getDevName(s);
#endif
}

bool DevHandler::setDevSN(const string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->setDevSN(s);
#endif
}

bool DevHandler::getDevSN(string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->getDevSN(s);
#endif
}

bool DevHandler::setDevFireWareVer(const string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->setDevFireWareVer(s);
#endif
}

bool DevHandler::getDevFireWareVer(string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->getDevFireWareVer(s);
#endif
}

bool DevHandler::setDevHardWareVer(const string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->setDevHardWareVer(s);
#endif
}

bool DevHandler::getDevHardWareVer(string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->getDevHardWareVer(s);
#endif
}

bool DevHandler::setDevSoftWareVer(const string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->setDevSoftWareVer(s);
#endif
}

bool DevHandler::getDevSoftWareVer(string &s)
{
#ifdef WINDOW_SYS       //add for windows
    return true;
#else
    return SonixHandler::instance()->getDevSoftWareVer(s);
#endif
}


string DevHandler::decToString(int iNum)
{
#define bitNum 8

    int m = iNum;
    char s[bitNum];
    char ss[bitNum];
    int i=0,j=0;
    if (iNum < 0)// 处理负数
    {
        m = 0 - m;
        j = 1;
        ss[0] = '-';
    }
    while (m>0)
    {
        s[i++] = m % 10 + '0';
        m /= 10;
    }
    s[i] = '\0';
    i = i - 1;
    while (i >= 0)
    {
        ss[j++] = s[i--];
    }
    ss[j] = '\0';

    return ss;
}

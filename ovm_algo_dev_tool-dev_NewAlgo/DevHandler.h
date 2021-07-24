#ifndef DEVHANDLER_H
#define DEVHANDLER_H

#include "device-linux/sonixhandler.h"

class DevHandler
{
public:
    DevHandler();
    ~DevHandler();

    static DevHandler *instance();

    bool init();
    bool unInit();

    bool setExposure(const int val);
    bool getExposure(int& val);

    bool setLEDCurrent(const int val);
    bool getLEDCurrent(int& val);

    bool setBLOpen(const bool flag);

    bool getDistance(int& val);
    //flash
    bool readFromFlash();
    bool write2Flash();

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
    string decToString(int iNum);
};

#endif // DEVHANDLER_H

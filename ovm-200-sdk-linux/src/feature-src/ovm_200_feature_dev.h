#ifndef OVM_200_FEATURE_DEV_H
#define OVM_200_FEATURE_DEV_H

#include <string>

using namespace std;

class FeatureDevelopTool{
public:
    FeatureDevelopTool();
    ~FeatureDevelopTool();
    static FeatureDevelopTool *instance();

    int getFeatureMax();
    void setFeatureMax(const int max);

    int getDistanceSensorLow();
    void setDistanceSensorLow(const int low);

    int getDistanceSensorHigh();
    void setDistanceSensorHigh(const int hi);

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
};

enum DevReturnValue{
    ERR_SUCC,
};

#endif

#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>
#include <map>

#include "ovm_200_match.h"

using namespace std;

/*******************************************************************/
//defines
enum MATCH_SDK_STATUS{
    e_null,
    e_1v1,
    e_1vN
};

//const value
const int conCompareTimes           = 3;
const int conMatchDisMarkStrSize    = 4;
const float conSysMemCapcityLimited = 0.9f;
const int conTemplateBytesCount     = 2304;
const int conIMatchThreadCount      = 3;

const int conTemplateFeatureSize = 5;
const int conTemplateCellBytes = 2304;
const int conTemplateCellFloatSize = 576;

//global value
extern bool g_bSdkInitFlag;
extern int g_iDisMark;
extern vector<int> g_vtrFeatureCheckSum;
extern int g_iMatchThreadCount;

extern OVS::MatchLevel g_eMatchLevel;
extern MATCH_SDK_STATUS g_eSDKStatus;
extern int g_i1V1Times;
extern map<string,int> g_map1vNTimes;


/*******************************************************************/

class Global{
public:
    Global();
    ~Global();
    static Global *instance();

    void checkDisMark(const string &str);
    bool isFeatureCheckSumExist(const string&str,int &checkSum);

    //
    bool checkSysMemCapacity();
    bool updateMap1vNTimes(const string &id);
private:
    int getCheckSum(const char *data,const int length);
    bool getMemCapacity(int &iTotal,int &iFree);
    int parseMemStr(const string &);
private:

};

#endif

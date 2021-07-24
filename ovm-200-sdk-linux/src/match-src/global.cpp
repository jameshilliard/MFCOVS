#include "global.h"
#include "logger.h"

#include <algorithm>

/*******************************************************************/
//global value
bool g_bSdkInitFlag = false;
int g_iDisMark = 0;
vector<int> g_vtrFeatureCheckSum;
int g_iMatchThreadCount = conIMatchThreadCount;
OVS::MatchLevel g_eMatchLevel = OVS::MatchLevel::lv_1;
MATCH_SDK_STATUS g_eSDKStatus = MATCH_SDK_STATUS::e_null;
int g_i1V1Times = 0;
map<string,int> g_map1vNTimes;
/*******************************************************************/
Global::~Global()
{

}

Global *Global::instance()
{
    static Global ins;
    return &ins;
}

void Global::checkDisMark(const string &str)
{
    string subStr = str.substr(
                str.size() - conMatchDisMarkStrSize,
                conMatchDisMarkStrSize);

    int tmpIDisMark = atoi(subStr.data());

    if(tmpIDisMark != g_iDisMark){
        g_iDisMark = tmpIDisMark;

        g_vtrFeatureCheckSum.clear();
        g_map1vNTimes.clear();
        g_i1V1Times = 0;
    }
}

bool Global::isFeatureCheckSumExist(const string &str, int &iCheckSum)
{
    iCheckSum = getCheckSum(str.data(),str.size());

    vector<int>::iterator it = find(g_vtrFeatureCheckSum.begin(),
                                    g_vtrFeatureCheckSum.end(),
                                    iCheckSum);
    //not find
    if(it == g_vtrFeatureCheckSum.end()){
        return false;
    }else{
        return true;
    }
}

bool Global::checkSysMemCapacity()
{
    logde<<"-----------------------";

    int iTotal = 0;
    int iFree = 0;

    getMemCapacity(iTotal,iFree);

    float ret = 1 - ((float)iFree / iTotal);

    logde<<"mem total:"<<iTotal<<",mem free:"<<iFree
        <<",used percent:"<<ret;

    if(ret > conSysMemCapcityLimited){
        return false;
    }else{
        return true;
    }
}


Global::Global()
{

}

int Global::getCheckSum(const char *data, const int length)
{
    if(data == nullptr){
        return 0;
    }
    if(length <= 0){
        return 0;
    }

    int sum = 0;
    for(int i = 0; i < length;i++){
        sum += (int)*(data + i);
    }

    return sum;
}


int Global::parseMemStr(const string & str)
{

    std::string::size_type iStartPos =
            str.find(":",0);

    string::size_type iEndPos =
            str.find("kB",0);

    string subStr = str.substr(
                iStartPos + 1,
                iEndPos - iStartPos - 2);

    return stoi(subStr);
}

bool Global::updateMap1vNTimes(const string &id)
{
    if(id.empty()){
        return false;
    }
    //
    auto it = g_map1vNTimes.find(id);

    if(g_map1vNTimes.end() != it){
        int times = it->second + 1;

        g_map1vNTimes[id] = times;
    }else{
        g_map1vNTimes[id] = 1;
    }
    //

    for(auto &it:g_map1vNTimes){
        if(it.second >= g_eMatchLevel){
            logde<<"sdk match,id:"<<it.first
                <<"|times:"<<it.second;

            return true;
        }
    }
    return false;
}

bool Global::getMemCapacity(int &iTotal, int &iFree)
{
    iTotal = 0;
    iFree = 0;

    ifstream infile("/proc/meminfo",std::ifstream::in);
    if(false == infile.is_open()){
        return false;
    }

    char chArray[100] = {'0'};
    //total
    infile.getline(chArray,100);
    string strTotal(chArray,100);
    iTotal = parseMemStr(strTotal);
    //free
    memset(chArray,0,100);
    infile.getline(chArray,100);
    iFree = parseMemStr(string(chArray,100));
    //    MemAvailable,discard
    infile.getline(chArray,100);
    //    Buffers
    memset(chArray,0,100);
    infile.getline(chArray,100);
    iFree += parseMemStr(string(chArray,100));
    //    Cached
    memset(chArray,0,100);
    infile.getline(chArray,100);
    iFree += parseMemStr(string(chArray,100));

    return true;
}

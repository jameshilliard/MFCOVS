#ifndef CACHE_HANDLER_H
#define CACHE_HANDLER_H

#include <string>
#include <map>
#include <vector>

#include "ovm_200_match.h"

using namespace std;

typedef vector<float> Vtrf;
typedef vector<vector<float> > vvtrf;
typedef vvtrf vtrArray;

typedef vector<string> vtrs;

typedef map<string,string> PalmMap;

class CacheHandler{
public:
    CacheHandler();
    static CacheHandler* instance();
    ~CacheHandler();

    void addLeftFeature(const string& id,const string& sTemplate);
    void addRightFeature(const string& id,const string& sTemplate);

    int getUserCount();

    bool delAllUser();

    bool delLeft(const string &id);
    bool deleRight(const string &id);

    bool idExistLeft(const string& id);
    bool idExistRight(const string& id);

    bool isLeftMapEmpty();
    bool isRightMapEmpty();
    //match
    bool matchVerifyByID(const string &feature, const string &userID);
    bool matchVerify(const string& ovsTemplate, const string &feature);

    bool matchIdentify(const OVS::MatchMode emm,const std::string &feature,std::string &userID);
private:
    void str2VtrTemplate(const string& str,Vtrf&vtr);
    void str2VvtrTemplate(const string& str,vvtrf &vtr);
    void str2VtrfFeature(const string& str,Vtrf& vtrf);

    void clearMap(PalmMap &pMap);

    bool matchIdentifyDetail(const PalmMap& palmMap,
                             const string &sFeature, string &userID);
private:
    PalmMap _leftMap;
    PalmMap _rightMap;
};

#endif

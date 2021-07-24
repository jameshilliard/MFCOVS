#include "cachehandler.h"
#include "algomatch.h"
#include "logger.h"
#include "global.h"

#include <cstring>



CacheHandler::~CacheHandler()
{

}

void CacheHandler::addLeftFeature(const string &id, const string &sTemplate)
{
    if(id.empty() || sTemplate.empty()){
        return ;
    }

    _leftMap[id] = sTemplate;
}

void CacheHandler::addRightFeature(const string &id, const string &sTemplate)
{
    if(id.empty() || sTemplate.empty()){
        return ;
    }

    _rightMap[id] = sTemplate;
}

int CacheHandler::getUserCount()
{
    int size = _leftMap.size() + _rightMap.size();

    for(auto it:_leftMap){
        auto re = _rightMap.find(it.first);
        if(_rightMap.end() != re){
            size--;
        }
    }

    return size;
}

bool CacheHandler::delAllUser()
{
    clearMap(_leftMap);
    clearMap(_rightMap);
    return true;
}

bool CacheHandler::delLeft(const string &id)
{
    auto it = _leftMap.find(id);
    if(_leftMap.end() != it){
        _leftMap.erase(it);

        return true;
    }else{
        return false;
    }
}

bool CacheHandler::deleRight(const string &id)
{
    auto it = _rightMap.find(id);
    if(_rightMap.end() != it){
        _rightMap.erase(it);

        return true;
    }else{
        return false;
    }
}

bool CacheHandler::idExistLeft(const string &id)
{
    auto it = _leftMap.find(id);
    if(_leftMap.end() != it){
        return true;
    }else{
        return false;
    }
}

bool CacheHandler::idExistRight(const string &id)
{
    auto it = _rightMap.find(id);
    if(_rightMap.end() != it){
        return true;
    }else{
        return false;
    }
}

bool CacheHandler::isLeftMapEmpty()
{
    if(_leftMap.size() == 0){
        return true;
    }else{
        return false;
    }
}

bool CacheHandler::isRightMapEmpty()
{
    if(_rightMap.size() == 0){
        return true;
    }else{
        return false;
    }
}

bool CacheHandler::matchVerifyByID(
        const string &feature, const string &userID)
{
    if(feature.empty() || userID.empty()){
        return false;
    }

    for(auto &item:_leftMap){
        if(item.first == userID){
            if(matchVerify(item.second,feature)){
                return true;
            }
        }
    }

    for(auto &item:_rightMap){
        if(item.first == userID){
            if(matchVerify(item.second,feature)){
                return true;
            }
        }
    }

    return false;

#if 0
    //
    Global::instance()->checkDisMark(feature);
    //erase dismark
    string tmpFeature(feature,0,feature.size() - conMatchDisMarkStrSize);
    //
    int iChcekSum;
    if(Global::instance()->isFeatureCheckSumExist(
                tmpFeature,
                iChcekSum)){
        return false;
    }
    //
    Vtrf vtrfFeature;
    str2VtrfFeature(feature,vtrfFeature);

    auto pair = _leftMap.find(userID);
    if(_leftMap.end() != pair){

        Vtrf vvtrfTemplate = (Vtrf)(pair->second);
#if 1
        for(auto item:vvtrfTemplate){
            Vtrf vtrfTempFeature = (Vtrf)item;
            bool re = AlgoMatch::instance()->computeFeature(
                        vtrfFeature.data(),
                        vtrfTempFeature.data(),
                        conCellFeatureBytes);

            if(re){
                return true;
            }
        }
#endif
    }
    return false;
#endif
}

bool CacheHandler::matchVerify(const string &sTemplate,
                               const string &sFeature)
{
    if(g_eSDKStatus == MATCH_SDK_STATUS::e_null){
        g_eSDKStatus = MATCH_SDK_STATUS::e_1v1;
    }

    //
    Global::instance()->checkDisMark(sFeature);
    //erase dismark
    string tmpFeature(sFeature,0,sFeature.size() - conMatchDisMarkStrSize);
    //
    int iChcekSum;
    if(Global::instance()->isFeatureCheckSumExist(
                tmpFeature,
                iChcekSum)){
        return false;
    }
    //
    vtrArray vaTemplate;
    str2VvtrTemplate(sTemplate,vaTemplate);

    Vtrf vtrFeature;
    str2VtrTemplate(sFeature,vtrFeature);

    //    bool bRe = AlgoMatch::instance()->computeFeature(
    //                vtrTemplate.data(),
    //                vtrFeature.data(),
    //                conCellTemplateBytes);

    //
    bool bRe = false;
#if 1
    for(auto& item:vaTemplate){
        if(AlgoMatch::instance()->computeFeature(
                    item.data(),
                    vtrFeature.data(),
                    conTemplateCellFloatSize))
        {
            bRe = true;
            break;
        }
    }
#endif
//    for(int i = 0; i < vaTemplate.size(); i++){
//        Vtrf vtrfTemplate = vaTemplate.at(i);
//        if(AlgoMatch::instance()->computeFeature(
//                    vtrfTemplate.data(),
//                    vtrFeature.data(),
//                    conTemplateCellFloatSize))
//        {
//            bRe = true;
//            break;
//        }
//    }
    //
    if(bRe){
        if(g_eSDKStatus == MATCH_SDK_STATUS::e_1v1){
            g_i1V1Times++;

            if(g_i1V1Times >= g_eMatchLevel){
                logde<<"0104-sdk match,1v1 times:"<<g_i1V1Times;

                g_i1V1Times = 0;
                g_eSDKStatus = MATCH_SDK_STATUS::e_null;

                return true;
            }else{
                return false;
            }
        }else if(g_eSDKStatus == MATCH_SDK_STATUS::e_1vN){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }

#if 0
    //
    vvtrf vvtrTemplate;
    str2VvtrTemplate(ovsTemplate,vvtrTemplate);

    Vtrf vtrFeature;
    str2VtrfFeature(tmpFeature,vtrFeature);

    for(auto &item:vvtrTemplate){
        Vtrf vtrfTmp = (Vtrf)item;

        bool re = AlgoMatch::instance()->computeFeature(
                    vtrfTmp.data(),
                    vtrFeature.data(),
                    conCellFeatureBytes);

        if(re){
            g_vtrFeatureCheckSum.push_back(iChcekSum);

            if(g_vtrFeatureCheckSum.size() >= conCompareTimes){
#if 1
                logde<<"match dis mark:"<<g_iDisMark;
                logde<<"match check sum vtr size:"
                    <<g_vtrFeatureCheckSum.size();

                for(int i = 0; i < g_vtrFeatureCheckSum.size();i++){
                    logde<<"check sum:"<<g_vtrFeatureCheckSum[i];
                }
#endif
                return true;
            }
        }
    }
    return false;
#endif
}

#if 0
bool CacheHandler::matchIdentify(const string &feature, string &userID)
{
#if 0
    //
    Global::instance()->checkDisMark(feature);
    //erase dismark
    string tmpFeature(feature,0,feature.size() - conMatchDisMarkStrSize);
    //
    int iChcekSum;
    if(Global::instance()->isFeatureCheckSumExist(
                tmpFeature,
                iChcekSum)){
        return false;
    }
#endif
    //
    for(auto &item:_leftMap){
        //        pair<string,vvtrf> &pairTmp = (pair<string,vvtrf>)item;
        vvtrf &vvtrfTemplate = item.second;
        string str((char*)vvtrfTemplate.data(),vvtrfTemplate.size() * 4);
        if(matchVerify(str,feature)){
            userID = item.first;
            return true;
        }
    }
    for(auto &item:_rightMap){
        pair<string,vvtrf> pairTmp = (pair<string,vvtrf>)item;
        vvtrf &vvtrfTemplate = pairTmp.second;
        string str((char*)vvtrfTemplate.data(),vvtrfTemplate.size() * 4);
        if(matchVerify(str,feature)){
            userID = item.first;
            return true;
        }
    }
    //
    return false;
}
#endif

bool CacheHandler::matchIdentify(const OVS::MatchMode emm,
                                 const string &feature,
                                 string &userID)
{
    if(feature.empty()){
        return false;
    }

    g_eSDKStatus = MATCH_SDK_STATUS::e_1vN;
    g_i1V1Times = 0;

    static OVS::MatchMode sMatchMode = OVS::MatchMode::e_left_right;
    if(sMatchMode != emm){
        sMatchMode = emm;
        g_map1vNTimes.clear();
    }

    userID.clear();

    bool bRe = false;
    string tmpUserID;

    if(emm == OVS::MatchMode::e_left){
        logde<<"machIdentify deatail,left map working.";
        bRe = matchIdentifyDetail(_leftMap,feature,tmpUserID);
    }else if(emm == OVS::MatchMode::e_right){
        logde<<"machIdentify deatail,right map working.";
        bRe = matchIdentifyDetail(_rightMap,feature,tmpUserID);
    }else if(emm == OVS::MatchMode::e_left_right){
        logde<<"machIdentify deatail,left and right map working.";
        bool bLeftRe = matchIdentifyDetail(_leftMap,feature,tmpUserID);
        bool bRightRe = matchIdentifyDetail(_rightMap,feature,tmpUserID);

        if(bLeftRe || bRightRe){
            bRe = true;
        }
    }
    //
    if(bRe){
        if(Global::instance()->updateMap1vNTimes(tmpUserID)){
            userID = tmpUserID;
            g_eSDKStatus = e_null;

            return true;
        }
    }

    g_eSDKStatus = e_null;
    return false;
}

void CacheHandler::str2VtrTemplate(const string &str, Vtrf &vtr)
{
    if(str.empty()){
        return;
    }

    vtr.clear();
    vtr.resize(str.size());

    memcpy(vtr.data(),str.data(),str.size());
}

void CacheHandler::str2VvtrTemplate(const string &srcStr,
                                    vvtrf &featureVtr)
{
    if(srcStr.size() < conTemplateBytesCount){
        return;
    }

    featureVtr.clear();
    //item
    Vtrf vtrfTmp;
    for(int i = 0; i < conTemplateFeatureSize; i++){
        string str = srcStr.substr(0 + i * conTemplateCellBytes ,
                                   conTemplateCellBytes);

        vtrfTmp.clear();
        vtrfTmp.resize(conTemplateCellFloatSize);

        memcpy(vtrfTmp.data(),str.data(),str.size());

        featureVtr.push_back(vtrfTmp);

        //display data
#if 0
        logde<<"sdk,match,algo feature size:"<<vtrfTmp.size();

        for(int i = 0; i < vtrfTmp.size(); i++){
            logde<<"sdk,feature item:"
                <<i<<"|"
                <<vtrfTmp.at(i);
        }
#endif

    }

    //display data
#if 0
    for(int i = 0; i < featureVtr.size(); i++){
        Vtrf vtrfTemplate = featureVtr.at(i);
        for(int j = 0; j < vtrfTemplate.size();j++){
            logde<<"sdk feature,template data:"
                <<i<<"|"
                <<j<<"|"<<vtrfTemplate.at(j);
        }
    }
#endif
}

void CacheHandler::clearMap(PalmMap &pmap)
{
    pmap.clear();
}

void CacheHandler::str2VtrfFeature(const string &str, Vtrf &vtrf)
{
    int byteLength = str.size();

    vtrf.clear();
    vtrf.resize(byteLength / sizeof(float));

    memcpy(vtrf.data(),str.data(),str.size());
}

bool CacheHandler::matchIdentifyDetail(const PalmMap &palmMap,
                                       const string &sFeature,
                                       string &userID)
{
    //
    if(palmMap.size() < 1){
        return false;
    }

    //
    for(auto &item:palmMap){
        if(matchVerify(item.second,sFeature)){
            userID = item.first;
            return true;
        }
    }

    return false;
}

#if 0
bool CacheHandler::matchIdentifyDetail(
        const SinglePalmTemplateMap &pPalmMap,
        const string &feature,
        string &userID)
{
    userID.clear();

    if(feature.empty()){
        return false;
    }

    const string &sFeature = feature;
    Vtrf vtrFeature;
    str2VtrTemplate(sFeature,vtrFeature);

    for(auto &item:pPalmMap){
        Vtrf vtrTemplate = item.second;

#if 0
        bool bRe = AlgoMatch::instance()->computeFeature(
                    vtrTemplate.data(),
                    vtrFeature.data(),
                    conCellFeatureBytes);
#endif
        if(bRe){
            userID = item.first;
            return true;
        }
    }

#if 0
    for(auto &item:pPalmMap){
        //        pair<string,vvtrf> &pairTmp = (pair<string,vvtrf>)item;
        vvtrf vvtrfTemplate = item.second;
        string str((char*)vvtrfTemplate.data(),vvtrfTemplate.size() * 4);
        if(matchVerify(str,feature)){
            userID = item.first;
            return true;
        }
    }
#endif

    return false;

}
#endif
CacheHandler::CacheHandler()
{

}

CacheHandler *CacheHandler::instance()
{
    static CacheHandler ins;
    return &ins;
}

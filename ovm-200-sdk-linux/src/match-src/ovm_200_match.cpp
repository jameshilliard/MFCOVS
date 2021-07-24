#include "ovm_200_match.h"
#include "cachehandler.h"
#include "logger.h"
#include "global.h"

OVS::STATUS OVS::InitMatchSDK(const MatchLevel mlv)
{
    if((mlv < MatchLevel::lv_1)
            || (mlv > MatchLevel::lv_5)){
        return  OVS::ERR_INVALID_PARAM;
    }

    g_bSdkInitFlag = true;

    g_eMatchLevel = mlv;

    Logger::instance();

    CacheHandler::instance();

    return OVS::SUCCESS;
}

OVS::STATUS OVS::MatchVerify(const string &ovsTemplate, const string &feature)
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    //param check
    if(ovsTemplate.empty() || feature.empty()){
        return  OVS::ERR_INVALID_PARAM;
    }
    //
    if(CacheHandler::instance()->matchVerify(ovsTemplate,feature)){
        return OVS::SUCCESS;
    }else{
        return OVS::ERR_MATCH_FAILED;
    }
}

int OVS::UserCount()
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    return CacheHandler::instance()->getUserCount();
}

OVS::STATUS OVS::AddUser(
        const string userID,
        const std::string &templateLeft,
        const std::string &templateRight)
{
    logde<<"sdk,match,add user:"
        <<userID;

    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    //param check
    if(userID.empty()){
        return OVS::ERR_INVALID_PARAM;
    }

    if(templateLeft.empty() && templateRight.empty()){
        return OVS::ERR_INVALID_PARAM;
    }
    //id exist
    if(false == templateLeft.empty()){
        if(CacheHandler::instance()->idExistLeft(userID)){
            return OVS::ERR_USER_LEFT_FEATURE_EXISTED;
        }
    }

    if(false == templateRight.empty()){
        if(CacheHandler::instance()->idExistRight(userID)){
            return OVS::ERR_USER_RIGHT_FEATURE_EXISTED;
        }
    }

    //memory check
    if(false == Global::instance()->checkSysMemCapacity()){
        return OVS::ERR_INSUFFICIENT_MEMORY;
    }

    //
    if(false == templateLeft.empty()){
        CacheHandler::instance()->addLeftFeature(userID,templateLeft);
    }
    if(false == templateRight.empty()){
        CacheHandler::instance()->addRightFeature(userID,templateRight);
    }

    return OVS::SUCCESS;
}

OVS::STATUS OVS::UpdateUser(const string userID,
                            const string &templateLeft,
                            const string &templateRight)
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    //param check
    if(userID.empty()){
        return OVS::ERR_INVALID_PARAM;
    }

    if(templateLeft.empty() && templateRight.empty()){
        return OVS::ERR_INVALID_PARAM;
    }
    //id exist
    if((false == CacheHandler::instance()->idExistLeft(userID))
            && (false == CacheHandler::instance()->idExistRight(userID)))
    {
        return OVS::ERR_USER_ID_NOT_EXIST;
    }
    //feature if is empty
    if(false == templateLeft.empty()){
        CacheHandler::instance()->addLeftFeature(userID,templateLeft);
    }
    if(false == templateRight.empty()){
        CacheHandler::instance()->addRightFeature(userID,templateRight);
    }

    return OVS::SUCCESS;
}

OVS::STATUS OVS::DeleteAllUsers()
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    CacheHandler::instance()->delAllUser();
    return OVS::SUCCESS;
}

OVS::STATUS OVS::DeleteUser(const string userID, const OVS::DeleteMode e)
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    //param check
    if(userID.empty()){
        return OVS::ERR_INVALID_PARAM;
    }
    //

    //
    switch (e) {
    case OVS::DeleteMode::e_left:{
        if(CacheHandler::instance()->isLeftMapEmpty()){
            return OVS::ERR_USER_COUNT_ZERO;
        }
        CacheHandler::instance()->delLeft(userID);
    }
        break;
    case OVS::DeleteMode::e_right:{
        if(CacheHandler::instance()->isRightMapEmpty()){
            return OVS::ERR_USER_COUNT_ZERO;
        }
        CacheHandler::instance()->deleRight(userID);
    }
        break;
    case OVS::DeleteMode::e_left_right:{
        if(CacheHandler::instance()->isLeftMapEmpty()
                && CacheHandler::instance()->isRightMapEmpty()){
            return OVS::ERR_USER_COUNT_ZERO;
        }
        CacheHandler::instance()->delLeft(userID);
        CacheHandler::instance()->deleRight(userID);
    }
        break;
    default:
        break;
    }

    return OVS::SUCCESS;
}

//OVS::STATUS OVS::MatchIdentify(const string &feature)
//{
//    //param check
//    if(feature.empty()){
//        return OVS::ERR_INVALID_PARAM;
//    }
//}

OVS::STATUS OVS::MatchVerifyByID(const string &feature,
                                 const string &userID)
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    //
    if(userID.empty() || feature.empty()){
        return OVS::ERR_INVALID_PARAM;
    }
    //
    if(CacheHandler::instance()->matchVerifyByID(feature,userID)){
        return OVS::SUCCESS;
    }else{
        return OVS::ERR_MATCH_FAILED;
    }

}

OVS::STATUS OVS::MatchIdentify(const OVS::MatchMode emm,
                               const string &feature,
                               string &userID)
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }

    userID.clear();

    if((emm < MatchMode::e_left)
            || (emm > MatchMode::e_left_right)){
        return OVS::STATUS::ERR_INVALID_PARAM;
    }

    if(feature.empty()){
        return OVS::STATUS::ERR_INVALID_PARAM;
    }
    //
    if(CacheHandler::instance()->isLeftMapEmpty()
            && CacheHandler::instance()->isRightMapEmpty()){
        return OVS::STATUS::ERR_USER_COUNT_ZERO;
    }

    if(CacheHandler::instance()->matchIdentify(emm,feature,userID)){
        return OVS::SUCCESS;
    }else{
        return OVS::ERR_MATCH_FAILED;
    }
}

OVS::STATUS OVS::UninitMatchSDK()
{
    if(false == g_bSdkInitFlag){
        return OVS::ERR_SDK_UNINITIALIZED;
    }else{
        g_bSdkInitFlag = false;
    }

    CacheHandler::instance()->delAllUser();
    return OVS::SUCCESS;
}

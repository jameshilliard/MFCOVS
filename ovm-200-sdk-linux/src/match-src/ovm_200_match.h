#ifndef OVS_MATCH_H
#define OVS_MATCH_H

#include <string>
#include "ovm_200_defines.h"

using namespace std;

namespace OVS {

enum MatchLevel{
    lv_1 = 1,    //
    lv_2,
    lv_3,
    lv_4,
    lv_5     //FAR min
};

enum DeleteMode{
    e_left,
    e_right,
    e_left_right
};
typedef DeleteMode MatchMode;

/**
 * @brief StartFeatureForMatch
 * @param call
 * @return
 */
STATUS InitMatchSDK(const MatchLevel mlv = MatchLevel::lv_1);
STATUS UninitMatchSDK();

/**
* @brief MatchVerify 1:1
* @param featureSrc
* @param featureDest
* @return
*/
STATUS MatchVerifyByID(const std::string &feature,const std::string &userID);
STATUS MatchVerify(const std::string &ovsTemplate,const std::string &feature);

/**
 * @brief MatchIdentify 1:N
 * @param feature
 * @param userID
 * @return
 */
STATUS MatchIdentify(const MatchMode emm,const std::string &feature,std::string &userID);

/**
 * @brief UserCount
 * @return
 */
int UserCount();
/**
 * @brief AddUser
 * @param userID
 * @param featureLeft
 * @param featureRight
 * @return
 */
STATUS AddUser(const std::string userID,const std::string &templateLeft,const std::string &templateRight);
/**
 * @brief UpdateUser
 * @param userID
 * @param featureLeft
 * @param featureRight
 * @return
 */
STATUS UpdateUser(const std::string userID,const std::string &templateLeft,const std::string &templateRight);

/**
 * @brief DeleteUser
 * @param userID
 * @return
 */
STATUS DeleteUser(const std::string userID,const DeleteMode e);
/**
 * @brief DeleteAllUsers
 * @return
 */
STATUS DeleteAllUsers();

}

#endif

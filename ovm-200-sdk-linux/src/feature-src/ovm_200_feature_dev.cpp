#ifdef MODE_DEV

#include "../ovm_200_defines.h"
#include "ovm_200_feature_dev.h"
#include "global.h"
#include "devhandler.h"

FeatureDevelopTool::FeatureDevelopTool()
{

}

FeatureDevelopTool::~FeatureDevelopTool()
{

}

FeatureDevelopTool *FeatureDevelopTool::instance()
{
    static FeatureDevelopTool ins;
    return &ins;
}

int FeatureDevelopTool::getFeatureMax()
{
    return g_iFeatureMax;
}

void FeatureDevelopTool::setFeatureMax(const int max)
{
    g_iFeatureMax = max;
}

int FeatureDevelopTool::getDistanceSensorLow()
{
    return g_iDisSensorLow;
}

void FeatureDevelopTool::setDistanceSensorLow(const int low)
{
    g_iDisSensorLow = low;
}

int FeatureDevelopTool::getDistanceSensorHigh()
{
    return g_iDisSensorHigh;
}

void FeatureDevelopTool::setDistanceSensorHigh(const int hi)
{
    g_iDisSensorHigh = hi;
}

bool FeatureDevelopTool::readFromFlash()
{
    return DevHandler::instance()->readFromFlash();
}

bool FeatureDevelopTool::write2Flash()
{
    return DevHandler::instance()->write2Flash();
}

bool FeatureDevelopTool::setDevName(const string &s)
{
    return DevHandler::instance()->setDevName(s);
}

bool FeatureDevelopTool::getDevName(string &s)
{
    return DevHandler::instance()->getDevName(s);
}

bool FeatureDevelopTool::setDevSN(const string &s)
{
    return DevHandler::instance()->setDevSN(s);
}

bool FeatureDevelopTool::getDevSN(string &s)
{
    return DevHandler::instance()->getDevSN(s);
}

bool FeatureDevelopTool::setDevFireWareVer(const string &s)
{
    return DevHandler::instance()->setDevFireWareVer(s);
}

bool FeatureDevelopTool::getDevFireWareVer(string &s)
{
    return DevHandler::instance()->getDevFireWareVer(s);
}

bool FeatureDevelopTool::setDevHardWareVer(const string &s)
{
    return DevHandler::instance()->setDevHardWareVer(s);
}

bool FeatureDevelopTool::getDevHardWareVer(string &s)
{
    return DevHandler::instance()->getDevHardWareVer(s);
}

bool FeatureDevelopTool::setDevSoftWareVer(const string &s)
{
    return DevHandler::instance()->setDevSoftWareVer(s);
}

bool FeatureDevelopTool::getDevSoftWareVer(string &s)
{
    return DevHandler::instance()->getDevSoftWareVer(s);
}

#endif

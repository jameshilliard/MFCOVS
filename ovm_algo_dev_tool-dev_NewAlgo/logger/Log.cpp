#include "logger/Log.h"
#include "logger/log4qt/propertyconfigurator.h"
#include "logger/log4qt/basicconfigurator.h"

Log *Log::instance()
{
    static Log ins;
    return &ins;
}

Log4Qt::Logger *Log::getLogPoint(int type)
{
    return pLog;
}

Log::Log()
{
    Log4Qt::BasicConfigurator::configure();
    pLog = Log4Qt::Logger::rootLogger();
    pLog->debug("debug!");
    pLog->info("information!");
    pLog->warn("warn");
//    pLog->log(logLevel, data);
}

Log::~Log()
{

}


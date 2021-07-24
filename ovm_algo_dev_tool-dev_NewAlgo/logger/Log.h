#ifndef			__LOG_H__
#define			__LOG_H__

#include "logger/log4qt/logger.h"

enum
{
    LOG_MODULE_WIDGET = 1,
    LOG_MODULE_DBMATCH
};

enum
{
    LOG_DEBUG = 1,
    LOG_INFO,
    LOG_WARING,
    LOG_ERROR
};

class Log
{    
public:
	Log();
	~Log();

    static Log *instance();
    Log4Qt::Logger *getLogPoint(int type);

private:
    Log4Qt::Logger *pLog;

};


#endif

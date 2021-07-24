#ifndef LOGGER_H
#define LOGGER_H

#include "easylogging++.h"

#define logde LOG(DEBUG)
#define loger LOG(ERROR)
#define logfa LOG(FATAL)

class Logger 
{
public:
    explicit Logger();
    static Logger *instance();
};

#endif // LOGGER_H

#include "logger.h"

INITIALIZE_EASYLOGGINGPP

Logger::Logger()
{
    el::Configurations conf("log.conf");
    el::Loggers::reconfigureAllLoggers(conf);
}

Logger *Logger::instance()
{
    static Logger ins;
    return &ins;
}

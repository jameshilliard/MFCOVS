#include "ovm_200_match_dev.h"
#include "../ovm_200_defines.h"
#include "ovm_200_match.h"
#include "global.h"

MatchDevelopTool::MatchDevelopTool()
{

}

MatchDevelopTool::~MatchDevelopTool()
{

}

MatchDevelopTool *MatchDevelopTool::instance()
{
    static MatchDevelopTool ins;
    return &ins;
}

int MatchDevelopTool::getMatchThreadCount()
{
    return g_iMatchThreadCount;
}

void MatchDevelopTool::setMatchThreadCount(const int count)
{
    if(count > 0){
        g_iMatchThreadCount = count;
    }
}

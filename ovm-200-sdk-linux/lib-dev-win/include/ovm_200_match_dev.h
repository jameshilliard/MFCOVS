#ifndef OVM_200_MATCH_DEV_H
#define OVM_200_MATCH_DEV_H

class MatchDevelopTool{
public:
    MatchDevelopTool();
    ~MatchDevelopTool();
    static MatchDevelopTool* instance();

    int getMatchThreadCount();
    void setMatchThreadCount(const int count);

};

#endif

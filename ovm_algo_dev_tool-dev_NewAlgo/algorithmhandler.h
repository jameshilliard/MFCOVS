#ifndef ALGORITHMHANDLER_H
#define ALGORITHMHANDLER_H


#include "opencv2/opencv.hpp"
#include "algorithm/include/utils.hpp"
#include "algorithm/include/palm_vein.hpp"
#include "Global.h"

typedef unsigned char uint8_t;
using namespace std;

class AlgoMatch{
public:
   static AlgoMatch* instance();
    AlgoMatch();
    ~AlgoMatch();

    bool computeFeature(const float* src, const float* dst, uint32_t len,float &hamming);
    bool computeFeature(const Vtrf &src, const Vtrf &dst, uint32_t len,float &hamming);
    bool computeFeature(const Vtrf &src, const Vtrf &dst, int &SuccHamming, int &FailHamming);

private:
    OVS::PalmVein *pv;
};

class AlgorithmHandler
{
public:
    explicit AlgorithmHandler();
    ~AlgorithmHandler();
    static AlgorithmHandler* instance();

    bool execute(const cv::Mat& mat,PalmImageInfo& info,std::string &feature,AuxDev::StatusInfo &_statusInfo);

    bool execute(const cv::Mat*,PalmImageInfo* info,std::vector<float>&dataVtr,AuxDev::StatusInfo &_statusInfo);

    int extract_status(AuxDev::StatusInfo& info);

    void ClearStatusInfo();

private:
    OVS::PalmVein *pv;
};

#endif // ALGORITHMHANDLER_H

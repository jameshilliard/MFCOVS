#ifndef ALGORITHMHANDLER_H
#define ALGORITHMHANDLER_H

#include "opencv2/opencv.hpp"
#include "utils.hpp"
#include "palm_vein.hpp"
#include "global.h"

typedef unsigned char uint8_t;
using namespace std;

class AlgorithmHandler
{
public:
    explicit AlgorithmHandler();
    ~AlgorithmHandler();
    static AlgorithmHandler* instance();

    bool execute(const cv::Mat& mat,
                 PalmImageInfo& info,
                 std::string &feature);

    bool execute(const cv::Mat*,
                 PalmImageInfo* info,
                 std::vector<float>&dataVtr);

};

#endif // ALGORITHMHANDLER_H

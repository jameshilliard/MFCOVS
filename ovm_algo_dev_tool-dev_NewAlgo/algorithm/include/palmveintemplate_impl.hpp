#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>

namespace OVS
{


class FeatureList_Float32
{
public:
    std::vector<cv::Point2f> kps;
    cv::Mat descriptor;
    static int match(const FeatureList_Float32 &a, const FeatureList_Float32 &b,
                     std::vector<std::pair<int, int>> &match,
                     double thresh)
    {
        auto matcher = cv::BFMatcher::create(4, true);
        std::vector<cv::DMatch> result;
        matcher->match(a.descriptor, b.descriptor, result);

        for (auto m : result)
        {
            if (m.distance > thresh)
                continue;
            match.push_back(std::make_pair(m.queryIdx, m.trainIdx));
        }

        return (int)match.size();
    }
};

class FeatureFrame;
class FeaturePoint
{
public:
    std::map<FeatureFrame *, uint32_t> visible_map;
    bool stable = false;

    std::map<FeatureFrame*, uint32_t> covisible_map;
    cv::Mat descriptor;
};

class FeatureFrame
{
public:
    uint32_t index;
    std::string type;
    std::string image_id;
    std::vector<FeaturePoint *> points;
    FeatureList_Float32 feature_list;
    std::map<FeatureFrame *, cv::Mat> H_map;

    //parameters for stable frame
    int stable_pts_;
};

class PalmVeinTemplate_impl
{
public:
    explicit PalmVeinTemplate_impl(float matching_threshold = 300.0f, float inlier_threshold = 3.0f,
                                    int stable_point_threshold = 10, int stable_pts_param=20);
    virtual ~PalmVeinTemplate_impl();
    void feed(const float* feat, uint32_t len);
    int check(bool &ok);
    bool build_template(std::vector<float>& temp);
private:

    void add_new_frame(std::vector<cv::Point2f>& kps, cv::Mat& descriptors);
    int search_stable_points();
    bool model_ready_ = false;
    std::list<FeatureFrame *> frames;
    std::list<FeaturePoint *> feature_points;
    std::vector<cv::Point2f> template_kps;
    cv::Mat template_descriptors;
    float thresh_matching;
    float thresh_inlier_checking;
    int thresh_stable_point;
    int param_stable_pts;
};

}
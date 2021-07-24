/**
 * 
 * 
 * 
 * 
 **/

#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "algorithm/include/utils.hpp"
#include "algorithm/include/palmveintemplate_impl.hpp"

using namespace std;
using namespace cv;

namespace OVS
{
PalmVeinTemplate_impl::PalmVeinTemplate_impl(float matching_threshold, float inlier_threshold,
                                            int stable_point_threshold, int stable_pts_param)
    : thresh_matching(matching_threshold), thresh_inlier_checking(inlier_threshold),
      thresh_stable_point(stable_point_threshold), param_stable_pts(stable_pts_param)
{
}

PalmVeinTemplate_impl::~PalmVeinTemplate_impl()
{
    for (auto ptr : frames)
    {
        delete ptr;
    }

    for (auto ptr : feature_points)
    {
        delete ptr;
    }
}

void PalmVeinTemplate_impl::feed(const float *feat, uint32_t len)
{
    const int desc_len = 128;
    const int feature_len = 2 + desc_len;
    const int feature_num = len / feature_len;
    vector<Point2f> kps(feature_num);
    Mat desc(Size(128, feature_num), CV_32FC1);

    int idx = 0;
    for (int i = 0; i < feature_num; ++i)
    {
        Point2f &pt = kps[i];
        pt.x = feat[idx++];
        pt.y = feat[idx++];
        auto ptr = desc.ptr<float>(i);
        for (int j = 0; j < desc_len; ++j)
        {
            ptr[j] = feat[idx++];
        }
    }

    add_new_frame(kps, desc);
}

int PalmVeinTemplate_impl::check(bool &ok)
{
    int n_stable = search_stable_points();
    ok = (n_stable > param_stable_pts);

    return n_stable;
}

bool PalmVeinTemplate_impl::build_template(std::vector<float> &tem)
{
    if(template_kps.empty())
        return false;

    const int feature_len = 2 + 128;
    tem.clear();
    tem.reserve(template_kps.size() * feature_len); 
    for(size_t i = 0; i < template_kps.size(); ++i)
    {
        tem.push_back(template_kps[i].x);
        tem.push_back(template_kps[i].y);
        auto ptr = template_descriptors.ptr<float>(i);
        for(int j = 0; j < template_descriptors.cols; ++j)
        {
            tem.push_back(ptr[j]);
        }
    }

    return true;
}

void PalmVeinTemplate_impl::add_new_frame(std::vector<cv::Point2f> &kps, cv::Mat &descriptors)
{
    FeatureFrame *new_frame = new FeatureFrame;
    FeatureList_Float32 feature_list{kps, descriptors};
    new_frame->feature_list = feature_list;

    map<int, FeaturePoint *> first_visible;

    for (auto frame : frames)
    {
        auto feat_list = frame->feature_list;
        vector<pair<int, int>> matches;
        int matched = FeatureList_Float32::match(feature_list, feat_list, matches, thresh_matching);
        if (matched <= 4)
            continue;
        vector<Point2f> pts0, pts1;
        for (auto p : matches)
        {
            pts0.push_back(feature_list.kps[p.first]);
            pts1.push_back(feat_list.kps[p.second]);
        }
        vector<Point3f> homo_pts0, est_homo_pts1;
        vector<Point2f> est_pts1;
        Mat H = findHomography(pts0, pts1, RANSAC, thresh_inlier_checking);
        vector<pair<int, int>> inliers;
        if (!H.empty())
        {
            convertPointsToHomogeneous(pts0, homo_pts0);
            for (auto pt : homo_pts0)
            {
                Mat v(Size(1, 3), CV_64FC1);
                v.at<double>(0) = pt.x;
                v.at<double>(1) = pt.y;
                v.at<double>(2) = pt.z;
                Mat h = H * v;
                Point3f hp;
                hp.x = h.at<double>(0);
                hp.y = h.at<double>(1);
                hp.z = h.at<double>(2);
                est_homo_pts1.push_back(hp);
            }
            convertPointsFromHomogeneous(est_homo_pts1, est_pts1);

            for (size_t i = 0; i < pts0.size(); ++i)
            {
                Point2f diff = pts1[i] - est_pts1[i];
                double v = sqrt(diff.ddot(diff));
                if (v < thresh_inlier_checking)
                {
                    inliers.push_back(matches[i]);
                }
            }
            frame->H_map.emplace(new_frame, H.inv());
            new_frame->H_map.emplace(frame, H);
        }
        for (auto match : inliers)
        {
            if (first_visible.find(match.first) == first_visible.end())
            {
                auto fpp = frame->points[match.second];
                first_visible.emplace(match.first, fpp);
            }
        }
    }

    for (int k = 0; k < (int)new_frame->feature_list.kps.size(); ++k)
    {
        if (first_visible.find(k) == first_visible.end())
        {
            FeaturePoint *pt = new FeaturePoint;
            pt->visible_map.emplace(new_frame, k);
            new_frame->points.push_back(pt);
            feature_points.push_back(pt);
        }
        else
        {
            auto pt = first_visible[k];
            pt->visible_map.emplace(new_frame, k);
            if (pt->visible_map.size() >= thresh_stable_point)
                pt->stable = true;
            new_frame->points.push_back(pt);
        }
    }

    frames.push_back(new_frame);

}

int PalmVeinTemplate_impl::search_stable_points()
{
    FeatureFrame *sig_frame_ptr = nullptr;
    int max_frame_cov = 0, pt_cov = 0;
    map<FeatureFrame *, int> sig_cov_map;
    for (auto frame : frames)
    {
        int stable_pts = 0;
        map<FeatureFrame *, int> cov;
        for (auto pt : frame->points)
        {
            stable_pts += pt->stable ? 1 : 0;
            for (auto p : pt->visible_map)
            {
                if (cov.find(p.first) == cov.end())
                {
                    cov.emplace(p.first, 0);
                }
                cov[p.first]++;
            }
        }
        int stable_cov = 0;
        for (auto p : cov)
        {
            if (p.second > 10) //TODO: hard-coded threshold
                stable_cov++;
        }
        if (stable_cov >= max_frame_cov)
        {
            if (stable_cov == max_frame_cov && stable_pts <= pt_cov)
                continue;
            sig_frame_ptr = frame;
            max_frame_cov = stable_cov;
            pt_cov = stable_pts;
            sig_cov_map = move(cov);
        }
    }

    list<FeatureFrame *> cov_list;
    for (auto p : sig_cov_map)
    {
        if (p.second >= 10) //TODO: hard-coded threshold
            cov_list.push_back(p.first);
    }

    map<FeaturePoint *, list<Point2f>> remap_pts;
    for (size_t k = 0; k < sig_frame_ptr->points.size(); ++k)
    {
        auto pt = sig_frame_ptr->points[k];
        if (!pt->stable)
            continue;
        pt->covisible_map.emplace(sig_frame_ptr, pt->visible_map[sig_frame_ptr]);
        remap_pts.emplace(pt, list<Point2f>());
        remap_pts[pt].push_back(sig_frame_ptr->feature_list.kps[k]);
    }
    for (auto frame : cov_list)
    {
        if (frame == sig_frame_ptr)
            continue;
        const Mat &H = frame->H_map[sig_frame_ptr];
        vector<Point3f> homo_pts0, est_homo_pts1;
        vector<Point2f> est_pts1;
        convertPointsToHomogeneous(frame->feature_list.kps, homo_pts0);
        for (auto pt : homo_pts0)
        {
            Mat v(Size(1, 3), CV_64FC1);
            v.at<double>(0) = pt.x;
            v.at<double>(1) = pt.y;
            v.at<double>(2) = pt.z;
            Mat h = H * v;
            Point3f hp;
            hp.x = h.at<double>(0);
            hp.y = h.at<double>(1);
            hp.z = h.at<double>(2);
            est_homo_pts1.push_back(hp);
        }
        convertPointsFromHomogeneous(est_homo_pts1, est_pts1);
        for (size_t i = 0; i < frame->points.size(); ++i)
        {
            auto pt = frame->points[i];
            if (!pt->stable)
                continue;
            if (remap_pts.find(pt) == remap_pts.end())
                remap_pts.emplace(pt, list<Point2f>());

            pt->covisible_map.emplace(frame, pt->visible_map[frame]);
            remap_pts[pt].push_back(est_pts1[i]);
        }
    }
    //
    {
        vector<pair<FeaturePoint *, int>> tmp_cmp;
        for (auto p : remap_pts)
        {
            tmp_cmp.push_back(make_pair(p.first, p.second.size()));
        }
        struct
        {
            bool operator()(const pair<FeaturePoint *, int> &a, const pair<FeaturePoint *, int> &b) const
            {
                return a.second < b.second;
            };
        } customLess;
        sort(tmp_cmp.begin(), tmp_cmp.end(), customLess);
        map<FeaturePoint *, list<Point2f>> tmp_remap_pts;
        for (auto p : tmp_cmp)
        {
            tmp_remap_pts[p.first] = remap_pts[p.first];
            if (tmp_remap_pts.size() >= 30)
                break;
        }
        remap_pts.clear();
        remap_pts = tmp_remap_pts;
    }

    //compute nearest descriptor
    for (auto p : remap_pts)
    {
        auto pt = p.first;
        int n = pt->covisible_map.size();
        Mat desc(Size(128, n), CV_32FC1);
        int row = 0;
        for (auto p : pt->covisible_map)
        {
            auto frame = p.first;
            uint32_t idx = p.second;
            const Mat &d = frame->feature_list.descriptor;
            for (int k = 0; k < 128; ++k)
            {
                desc.at<float>(row, k) = d.at<float>(idx, k);
            }
            ++row;
        }

        double min_dist = numeric_limits<double>::max();
        int min_idx = -1;
        for (int i = 0; i < row; ++i)
        {
            const Mat src = desc.row(i);
            double dist = 0;
            for (int j = 0; j < row; ++j)
            {
                if (j == i)
                    continue;
                const Mat dst = desc.row(j);
                for (int k = 0; k < 128; ++k)
                {
                    dist += fabs(dst.at<float>(k) - src.at<float>(k)) / 128.0;
                }
            }
            if (min_dist > dist)
            {
                min_dist = dist;
                min_idx = i;
            }
        }
        desc.row(min_idx).copyTo(pt->descriptor);
    }

    vector<Point2f> template_pts;
    Mat template_desc(Size(128, remap_pts.size()), CV_32FC1);
    int idx = 0;
    for (auto p : remap_pts)
    {
        auto feature_pt = p.first;
        auto pts = p.second;
        Point2f sum(0, 0), mean;
        for (auto pt : pts)
        {
            sum = sum + pt;
        }
        mean.x = sum.x / pts.size();
        mean.y = sum.y / pts.size();
        for (auto it = pts.begin(); it != pts.end();)
        {
            Point2f diff = mean - *it;
            if (diff.dot(diff) >= 5 * 5)
            {
                it = pts.erase(it);
            }
            else
            {
                ++it;
            }
        }
        if (pts.size() == 0)
            continue;
        sum.x = sum.y = 0;
        for (auto pt : pts)
        {
            sum = sum + pt;
        }
        mean.x = sum.x / pts.size();
        mean.y = sum.y / pts.size();
        template_pts.push_back(mean);
        feature_pt->descriptor.copyTo(template_desc.row(idx));
        ++idx;
    }

    template_kps = template_pts;
    template_desc.rowRange(0, idx).copyTo(template_descriptors);

    return (int)template_kps.size();
}

}

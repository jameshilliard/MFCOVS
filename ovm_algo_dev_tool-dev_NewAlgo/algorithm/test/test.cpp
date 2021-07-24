#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "palm_vein.hpp"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "USAGE: app input_media_path output_feature_path" << endl;
        return 0;
    }

    VideoCapture cap(argv[1]);
    if (!cap.isOpened())
    {
        cerr << "cannot open video file: " << argv[1] << endl;
        return 0;
    }

    list<vector<float>> feat_list;
    bool ok = true;
    int k = 0;
    while (ok)
    {
        Mat raw;
        cap >> raw;
        if (raw.cols == 0)
        {
            cout << "no frame" << endl;
            break;
        }
        k++;
        // cout << k << endl;

        Mat canvas;
        resize(raw, canvas, Size(960, 540), 0, 0);
        imshow("palm", canvas);

        int key = waitKey(10);
        if (key == 27)
        {
            ok = false;
        }

        OVS::PalmVein pv(raw.data, raw.cols, raw.rows, 3);
        pv.loadModelFiles("/media/jz/Data/palm_det.yaml",
                          "/media/jz/Data/finger_svm.yaml");
        PalmImageInfo info;
        auto rlt = pv.parseInfo(info);
        if (rlt != OVS_SUCCESS)
        {
            // cerr << "fail to parse palm infomation." << endl;
            continue;
        }

        std::cout << "palm info:" << std::endl
                  << info.center_x << ", " << info.center_y << ", " << info.mean_illumination << ", " << info.score << endl;
        if (info.mean_illumination < 70.0)
        {
            rlt = OVS_ERR_ROI_TOOWEAK;
            cerr << "roi is too weak." << endl;
            continue;
        }
        else if (info.mean_illumination > 180.0)
        {
            rlt = OVS_ERR_ROI_TOOSTRONG;
            cerr << "roi is too strong" << endl;
            continue;
        }
        uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
        rlt = pv.findRoi(nullptr, roi_width, roi_height, roi_byte_size);
        if (rlt != OVS_SUCCESS)
        {
            cerr << "fail to extract roi." << endl;
            continue;
        }
        uint8_t *roi_data = new uint8_t[roi_byte_size];
        rlt = pv.findRoi(roi_data, roi_width, roi_height, roi_byte_size);
        if (rlt != OVS_SUCCESS)
        {
            cerr << "fail to extract roi." << endl;
            delete[] roi_data;
            continue;
        }

        std::vector<float> feat;
        rlt = pv.computeFeature(feat);
        // cout << "feature computed" << endl;
        if(rlt == OVS_SUCCESS)
        {
            for(const auto& f : feat_list)
            {
                float d = OVS::PalmVein::compareFeature(&f[0], &feat[0], feat.size());
                cout << " " << d;
            }
            cout << endl;
            feat_list.push_back(feat);
            
        }
        delete[] roi_data;
    }

    ofstream ofs(argv[2]);
    for(const auto& feat : feat_list)
    {
        for(auto v : feat)
        {
            ofs << v << ",";
        }
        ofs << endl;
    }
    cout << "total " << feat_list.size() << " features" << endl;

    return 0;
}
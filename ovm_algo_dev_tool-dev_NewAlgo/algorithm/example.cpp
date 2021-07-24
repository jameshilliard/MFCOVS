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
#include <chrono>
using namespace std;
using namespace cv;




int extract_status(AuxDev::StatusInfo& info) {

    //A1 手掌检测
    {
        bool flag = info.a1_palm.success;
        float tm = info.a1_palm.time_consume;
    }

    //A2 指缝检测
    {
        bool flag = info.a2_finger.success;
        float tm = info.a2_finger.time_consume; 
    }

    //A3 ROI检测
    {
        bool flag = info.a3_roi.success;
        float tm = info.a3_roi.time_consume; 
    }

    //A4 预处理
    {
        bool flag = info.a4_enhance.success;
        float tm = info.a4_enhance.time_consume;
    }

    //A5 计算特征
    {
        bool flag = info.a5_fcompute.success;
        float tm = info.a5_fcompute.time_consume;
    }

    //显示图像
    Mat im_wnd;
    info.image_wnd.copyTo(im_wnd);
    Mat im_roi;
    info.image_roi.copyTo(im_roi);
    Mat im_svm;
    info.image_svm.copyTo(im_svm);

    return 1;
}

int main_example(int argc, char* argv[])
{
    string file = "./model/image/9-110.jpg";
    Mat raw = imread(file);

    //加载模型
    OVS::PalmVein pv(raw.data, raw.cols, raw.rows, 3);
    pv.loadModelFiles("./model/palm_hog.yaml", "./model/finger_hog_norm.yaml");
    PalmImageInfo info;
    auto rlt = pv.parseInfo(info);

    //图像合理性
    if (rlt != OVS_SUCCESS) {
        cerr << "fail to parse palm infomation." << endl;
        return 0;
    }
    if (info.mean_illumination < 70.0) {
        rlt = OVS_ERR_ROI_TOOWEAK;
        cerr << "roi is too weak." << endl;
        return 0;
    }
    else if (info.mean_illumination > 180.0) {
        rlt = OVS_ERR_ROI_TOOSTRONG;
        cerr << "roi is too strong" << endl;
        return 0;
    }

    //检测ROI区域
    uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
    rlt = pv.findRoi(nullptr, roi_width, roi_height, roi_byte_size);
    if (rlt != OVS_SUCCESS) {
        cerr << "fail to extract roi." << endl;
        return 0;
    }
    uint8_t* roi_data = new uint8_t[roi_byte_size];
    rlt = pv.findRoi(roi_data, roi_width, roi_height, roi_byte_size);
    if (rlt != OVS_SUCCESS) {
        cerr << "fail to extract roi." << endl;
        delete[] roi_data;
        return 0;
    }

    //计算特征
    std::vector<float> feat;
    rlt = pv.computeFeature(feat);
    if (rlt != OVS_SUCCESS) {
        cerr << "fail to extract feature." << endl;
        delete[] roi_data;
        return 0;
    }


    //获取显示数据
    pv.loadStatusInfo();//每次调用
    extract_status(pv._statusInfo); //示例


    return 0;
}


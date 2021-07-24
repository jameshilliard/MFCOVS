#include "algorithmhandler.h"
#include "palm_vein.hpp"
#include "logger.h"
#include "global.h"

#if defined(__linux__)      //add for windows
extern "C"{
#include <linux/videodev2.h>
}
#endif

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

AlgorithmHandler::AlgorithmHandler()
{
}

AlgorithmHandler::~AlgorithmHandler()
{
}

AlgorithmHandler *AlgorithmHandler::instance()
{
    static AlgorithmHandler ins;
    return &ins;
}

bool AlgorithmHandler::execute(
        const Mat &mat,
        PalmImageInfo &info,
        std::string &feature)
{
    std::vector<float> dataVtr;

    bool re = execute(&mat,&info,dataVtr);

    if(re){
        int byteLength = dataVtr.size() * sizeof(float);
        feature = string((char*)dataVtr.data(),byteLength);

#if 0
        logde<<"sdk,algo feature size:"<<byteLength;

        for(int i = 0; i < dataVtr.size(); i++)        {
            logde<<"sdk,feature item:"
                <<i<<"|"
                <<dataVtr.at(i);
        }
#endif
    }

    return re;
}

bool AlgorithmHandler::execute(
        const Mat *raw,
        PalmImageInfo *sInfo,
        std::vector<float>& dataVtr)
{
#if 1
    OVS::PalmVein pv(raw->data, raw->cols, raw->rows, 3);
#else

#if 0
    OVS::PalmVein pv(Global::instance()->_imageData,
                     1920,
                     1080,
                     CV_8UC3);

    free(Global::instance()->_imageData);
    Global::instance()->_imageData = nullptr;
#endif

    QImage image =
            Global::instance()->_preAlorithmImage.convertToFormat(
                QImage::Format_RGB32);

    image = image.convertToFormat(QImage::Format_BGR30);

    if(image.isNull()){
        logde<<"algorithm execute image is null.--------------";
        return false;
    }

    OVS::PalmVein pv((unsigned char*)image.bits(),
                     1920,
                     1080,
                     3);

#endif
    if(pv.loadModelFiles("./palm_hog.yaml",
                         "./finger_hog_norm.yaml")){
        //        logde<<"load model file succ.";
    }else{
        logde<<"load model file failed.";
        return e_err_load_file;
    }

    PalmImageInfo *info = sInfo;
    auto rlt = pv.parseInfo(*info);
    if (rlt != OVS_SUCCESS)
    {
        //        logde<<"parseInfo failed.----------------------------";
        info->center_x = -1;
        info->center_y = -1;
        info->score = -1;
        info->mean_illumination = -1;

        return e_err_parse_info;
    }else{
        //        logde<<"parseInfo succ.++++++++++++++++++++++++++++++";
    }

    if (info->mean_illumination < constIlluminationLow)
    {
        logde<<"algo,mean_illumination:"<<info->mean_illumination;
        rlt = OVS_ERR_ROI_TOOWEAK;
        //        logde<<"roi is too weak..----------------------------";
        //        return (int)info->mean_illumination;
        return false;
    }
    else if (info->mean_illumination > constIlluminationHigh)
    {
        logde<<"algo,mean_illumination:"<<info->mean_illumination;
        rlt = OVS_ERR_ROI_TOOSTRONG;
        //        logde<<"roi is too strong.---------------------------";
        //        return (int)info->mean_illumination;
        return false;
    }
    uint32_t roi_width = 0, roi_height = 0, roi_byte_size = 0;
    rlt = pv.findRoi(nullptr, roi_width, roi_height, roi_byte_size);
    if (rlt != OVS_SUCCESS)
    {
        //        logde<<"fail to extract roi..---------------------------";
        return e_err_find_roi;
    }
    uint8_t *roi_data = new uint8_t[roi_byte_size];
    rlt = pv.findRoi(roi_data, roi_width, roi_height, roi_byte_size);
    if (rlt != OVS_SUCCESS)
    {
        delete[] roi_data;
        //        logde<<"fail to extract roi.---------------------------.";
        return e_err_find_roi;
    }

    rlt = pv.computeFeature(dataVtr);

    if(rlt == OVS_SUCCESS)
    {
        delete[] roi_data;

        return e_succ;
#if 0
        for(const auto& f : feat_list)
        {
            auto hamming = OVS::PalmVein::compareFeature(&f[0], &feat[0], feat.size());
            cout << " " << hamming;
        }
        cout << endl;
        feat_list.push_back(feat);
#endif
    }else{
        delete[] roi_data;
        return e_err_compute_feature;
    }

}

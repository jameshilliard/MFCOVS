#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

void read_directory(const std::string& name, std::vector<std::string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if(strcmp(".", dp->d_name) == 0 || strcmp("..", dp->d_name) == 0)
            continue;
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

int main(int argc, char* argv[])
{
    string path = argv[1];
    string label = argv[2];
    ofstream ofs("/media/jz/Data/samples/newdata" + label + ".txt");
    vector<string> filelist;
    read_directory(path, filelist);
    cout << filelist.size() << endl; 
    for(auto& s : filelist)
    {
        s = path + s;
        cout << s << endl;
        Mat im = imread(s, 0);
        if(im.rows == 0)
            continue;
        
        // Rect roi(420, 0, 1080, 1080);
        Rect roi(35, 0, 90, 90);
        Mat dst;
        cv::resize(im(roi), dst, Size(128, 128));
        const cv::Size winSize(128, 128), blockSize(32, 32), blockStride(16, 16), cellSize(8, 8);
        int nBins = 4;
        const cv::Size winStride(32, 32), padding(0, 0);
        auto hog = cv::HOGDescriptor(winSize, blockSize, blockStride, cellSize, nBins);
        std::vector<float> desc;
        hog.compute(dst, desc, winStride, padding);
        const auto minmax = std::minmax_element(desc.begin(), desc.end());
        float min_v = *minmax.first, max_v = *minmax.second;
        auto range = max_v - min_v;

        stringstream ss;
        ss << label;
        for(auto& v : desc)
        {
            int a = (int)(255 * (v - min_v) / range);
            // cout << v << " ";
            ss << "," << a;
        }
        ofs << ss.str() << endl;
    }
    // cout << endl;


    // const string model_path = "/media/jz/Data/svm.yaml";
    // Ptr<ml::SVM> svm_ = cv::ml::SVM::load(model_path);
    // cv::FileStorage fs_(model_path, cv::FileStorage::READ);
    // cv::Mat EigenValues, EigenVectors, Mean;
    // fs_["eigenvalues"] >> EigenValues;
    // fs_["eigenvectors"] >> EigenVectors;
    // fs_["mean"] >> Mean;
    // PCA pca_;
    // pca_.eigenvalues = EigenValues.clone();
    // pca_.eigenvectors = EigenVectors.clone();
    // pca_.mean = Mean.clone();
    // Mat pca_mean_;
    // fs_["pca_mean"] >> pca_mean_;

    // cv::Mat data(1, desc.size(), CV_32F);
    // for(size_t i = 0; i < desc.size(); ++i)
    // {
    //     data.at<float>(i) = desc[i];
    // }    

    // cv::Mat projected = pca_.project(data);

    // // std::cout << projected << std::endl;
    // for(int j = 0; j < projected.cols; ++j)
    // {
    //     float mean = (pca_mean_.at<float>(0, j) + pca_mean_.at<float>(1, j)) / 2.0f;
    //     float range = (pca_mean_.at<float>(1, j) - pca_mean_.at<float>(0, j)) / 2.0f;
    //     projected.at<float>(j) = (projected.at<float>(j) - mean) / range;
    //     if(projected.at<float>(j) < -1.0)
    //         projected.at<float>(j) = -1.0;
    //     if(projected.at<float>(j) > 1.0)
    //         projected.at<float>(j) = 1.0;
    // }


    // cv::Mat rlt;
    // float v = svm_->predict(projected, rlt, cv::ml::SVM::RAW_OUTPUT);
    // std::cout << rlt << std::endl;

    return 0;
}
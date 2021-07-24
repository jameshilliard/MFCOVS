#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <random>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    ifstream ifs(argv[1]);
    vector<float> labels;
    vector<vector<float>> features;
    int feat_len = 0;
    while (!ifs.eof() && !ifs.bad())
    {
        string str;
        getline(ifs, str);
        if (str.size() <= 2)
            break;

        std::regex regex("\\,");

        std::vector<std::string> out(
            std::sregex_token_iterator(str.begin(), str.end(), regex, -1),
            std::sregex_token_iterator());

        vector<float> feat;
        size_t k = 0;
        labels.push_back(stof(out[0]));
        for (k = 1; k < out.size(); ++k)
        {
            if (out[0].size() < 1)
                continue;
            feat.push_back(stof(out[k]));
        }

        // const auto minmax = std::minmax_element(feat.begin(), feat.end());
        // float min_v = *minmax.first, max_v = *minmax.second;
        // auto range = max_v - min_v;
        // assert(range != 0);
        for(auto& v : feat)
        {
            v = v / 255.0;
        }

        if(feat_len == 0)
        {
            feat_len = (int)feat.size();
        }
        
        if(feat_len != (int)feat.size())
        {
            cout << " bad format " << endl;
        }
        else
        {
            features.push_back(feat);
        }
        
        cout << features.size() << ": " << feat.size() << endl;
        // if(features.size() > 7000)
        //     break;
    }
    cout << "---------------------------" << endl;

    vector<int> seeds;
    for(int i = 0; i < features.size(); i++)
        seeds.push_back(i);

    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(seeds.begin(), seeds.end(), g);

    Mat train_data(seeds.size(), feat_len, CV_32F);
    Mat label_data(seeds.size(), 1, CV_32S);
    for(size_t i = 0; i < train_data.rows; ++i)
    {
        int idx = seeds[i];
        float* ptr = train_data.ptr<float>(i);
        for(size_t j = 0; j < feat_len; ++j)
        {
            ptr[j] = features[idx][j];
        }
        label_data.at<int>(i) = (int)(labels[idx] + 0.1);
    }
    features.clear();
    cout << "==============================" << endl;

    // {
    PCA pca(train_data, Mat(), CV_PCA_DATA_AS_ROW, 20);

    cout << pca.eigenvalues << endl;

    // }
    
    // PCA pca_sample;
    // FileStorage fs_("/media/jz/Data/pca.yaml", FileStorage::READ);
    // Mat EigenValues, EigenVectors, Mean;
    // fs_["eigenvalues"] >> EigenValues;
    // fs_["eigenvectors"] >> EigenVectors;
    // fs_["mean"] >> Mean;
    // pca_sample.eigenvalues = EigenValues.clone();
    // pca_sample.eigenvectors = EigenVectors.clone();
    // pca_sample.mean = Mean.clone();

    // Mat pca_mean;
    // fs_["pca_mean"] >> pca_mean;

    Mat projected = pca.project(train_data);

    Mat pca_mean(2, projected.cols, CV_32F);
    vector<pair<float, float>> maxRange(projected.cols);
    for(int j = 0; j < projected.cols; j++)
    {
        float minv = numeric_limits<float>::max();
        float maxv = -numeric_limits<float>::max();
        for(int i = 0; i < projected.rows; i++)
        {
            if(projected.at<float>(i, j) > maxv)
                maxv = projected.at<float>(i, j);
            if(projected.at<float>(i, j) < minv)
                minv = projected.at<float>(i, j);
        }
        pca_mean.at<float>(0, j) = minv;
        pca_mean.at<float>(1, j) = maxv;
        maxRange[j] = make_pair(minv, maxv);
        cout << "[" << minv << ", " << maxv << "] " << " ";
    }
    cout << endl;

    // {
    //     FileStorage fs_("/media/jz/Data/pca.yaml", FileStorage::APPEND);
    //     fs_ << "pca_mean" << pca_mean;
    // }
    // cout << projected.rowRange(0, 5) << endl;

    for(int i = 0; i < projected.rows; ++i)
    {
        float* ptr = projected.ptr<float>(i);
        for(int j = 0; j < projected.cols; ++j)
        {
            float mean = (pca_mean.at<float>(0, j) + pca_mean.at<float>(1, j)) / 2.0f;
            float range = (pca_mean.at<float>(1, j) - pca_mean.at<float>(0, j)) / 2.0f;
            ptr[j] = (ptr[j] - mean) / range;
            if(ptr[j] < -1.0) ptr[j] = -1.0;
            if(ptr[j] > 1.0) ptr[j] = 1.0;
        }
    }

    {
    Ptr<ml::SVM> svm = ml::SVM::create();
    svm->setType(ml::SVM::C_SVC);
    svm->setC(0.1);
    svm->setKernel(ml::SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, (int)1e4, 1e-4));
    cout << "train svm" << endl;
    svm->train(projected, ml::ROW_SAMPLE, label_data);

    cout << "ok" << endl;
    svm->save("/media/jz/Data/svm.yaml");
    }
{
    FileStorage fs("/media/jz/Data/svm.yaml", FileStorage::APPEND);
    fs << "eigenvalues" << pca.eigenvalues;
    fs << "eigenvectors" << pca.eigenvectors;
    fs << "mean" << pca.mean;
    fs << "pca_mean" << pca_mean;
}


    Ptr<ml::SVM> svm = ml::SVM::load("/media/jz/Data/svm.yaml");
    Mat results;
    svm->predict(projected, results, ml::SVM::RAW_OUTPUT);
    // cout << results << endl;

    int p0 = 0, p1 = 0, p = 0, n0 = 0, n1 = 0;
    for(int i = 0; i < results.rows; ++i)
    {
        // cout << results.at<float>(i) << "->" << label_data.at<int>(i) << endl;
        if(results.at<float>(i) > 0 && label_data.at<int>(i) == 1)
            n0++;
        else if(results.at<float>(i) < 0 && label_data.at<int>(i) == 0)
            n1++;
        else if(results.at<float>(i) < 0 && label_data.at<int>(i) == 1)
            p0++;
        else if(results.at<float>(i) > 0 && label_data.at<int>(i) == 0)
            p1++;
        else p++;
    }

    cout << n0 << endl;
    cout << n1 << endl;
    cout << p0 << endl;
    cout << p1 << endl;
    cout << p << endl;
    return 0;
}
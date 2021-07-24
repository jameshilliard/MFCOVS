#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <regex>
#include <random>
#include <limits>
#include <algorithm>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/types.h>
#include <dirent.h>
#include "palm_vein.hpp"

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

void read_listfile(const std::string& name, std::vector<std::string>& v)
{
    ifstream ifs(name);
    while(!ifs.eof())
    {
        string str;
        getline(ifs, str);
        v.push_back(str);
    }
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "USAGE: app input_media_path output_feature_path" << endl;
        return 0;
    }

    string in_path = argv[1];
    string out_path = argv[2];
    vector<string> filelist;
    read_directory(in_path, filelist);
    // read_listfile(in_path, filelist);

    map<string, list<string>> sample_table;

    int total = 0, success = 0, fail = 0;
    for(const auto& str : filelist)
    {
        std::regex path_regex("\\/");
        std::vector<std::string> subs(
            std::sregex_token_iterator(str.begin(), str.end(), path_regex, -1),
            std::sregex_token_iterator());

        auto filename = subs.back();
        std::regex regex("\\_");

        std::vector<std::string> out(
            std::sregex_token_iterator(filename.begin(), filename.end(), regex, -1),
            std::sregex_token_iterator());

        if(out.size() != 5)
            continue;
        
        string id = out[2] + "_" + out[1];
        string filepath = in_path + "/" + str;
        // string filepath = str;
        if(sample_table.find(id) == sample_table.end())
        {
            list<string> item;
            item.push_back(filepath);
            sample_table.emplace(id, item);
        }
        else
        {
            sample_table[id].push_back(filepath);
        }
        total++;
    }

    cout << sample_table.size() << endl;
    list<string> failed_samples;
    for(const auto& item : sample_table)
    {
        string id = item.first;
        list<vector<float>> feats;
        auto& image_list = item.second;
        for(auto& path : image_list)
        {
            cout << path << endl;
            failed_samples.push_back(path);
            Mat raw = imread(path);
            if(raw.data == nullptr)
                continue;
        
            OVS::PalmVein pv(raw.data, raw.cols, raw.rows, 3);
            pv.loadModelFiles("/media/jz/Data/palm_samples/palm_hog.yaml",
                            "/media/jz/Data/palm_samples/finger_hog_norm.yaml");
                            
            PalmImageInfo info;
            auto rlt = pv.parseInfo(info);

            if (rlt != OVS_SUCCESS)
            {
                cerr << "fail to parse palm infomation (" << rlt << ")." << endl;
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
            else if (info.mean_illumination > 210.0)
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
            failed_samples.pop_back();

            std::vector<float> feat;
            rlt = pv.computeFeature(feat);
            // cout << "feature computed" << endl;
            if(rlt == OVS_SUCCESS)
            {
                feats.push_back(feat);
                success++;
            }
            delete[] roi_data;

            // ofstream ofs(out_path + "/" + id + ".txt");
            // for(const auto& feat : feats)
            // {
            //     for(auto v : feat)
            //     {
            //         ofs << v << ",";
            //     }
            //     ofs << endl;
            // }
        }
    }

    // {
    //     ofstream ofs(out_path + "/failed.txt");
    //     for(const auto& s : failed_samples)
    //     {
    //         ofs << s << endl;
    //     }
    // }

    cout << total << " " << success << " " << total - success << endl;

    return 0;
}
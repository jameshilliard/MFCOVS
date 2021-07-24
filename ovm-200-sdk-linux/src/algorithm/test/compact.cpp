#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iomanip>
#include <cmath>
#include <vector>
#include <regex>
#include <random>
#include <limits>
#include <algorithm>
#include <utility>
#include <sys/types.h>
#include <dirent.h>


using namespace std;

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


float compare_feature(const float* src, const float* dst, uint32_t len)
{
    double ms = 0, md = 0;
    double ssums = 0, ssumd = 0;
    double vars = 0, vard = 0;
    for(uint32_t i = 0; i < len; ++i)
    {
        ms += src[i];
        md += dst[i];
        ssums += src[i] * src[i];
        ssumd += dst[i] * dst[i];
    }
    ms /= len;
    md /= len;
    vars = (ssums / len) - ms*ms;
    vard = (ssumd / len) - md*md;
    double l2 = 0;
    double w = 1.0 / len / (sqrt(vars * vard)); //std::max(src[i], dst[i]);
    for(uint32_t i = 0; i < len; ++i)
    {
            double v = src[i] - dst[i];
            // double v = (src[i] - ms) * (dst[i] - md);
            l2 += v * v;
        // cout << sqrt(bl2) << " ";
    }
    // cout << endl;
    // double v = std::max(0.0, 1.0 - l2 * w);
    double v = sqrt(l2 / len);
    return (float)v;
}


bool load_features(const string& path, vector<vector<float>>& feature_list)
{
    ifstream ifs(path);
    if(!ifs.is_open())
        return false;

    size_t last_feat_len = 0;
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
        for(auto& s : out)
        {
            if(s.empty())
                continue;
            float v = stof(s);
            feat.push_back(v);
        }
        // cout << feat.size() << endl;
        if(last_feat_len != 0 && feat.size() != last_feat_len)
        {
            cerr << "bad format feature" << endl;
            continue;
        }
        last_feat_len = feat.size();

        feature_list.push_back(feat);
    }

    return feature_list.size() > 0;
}

void compact_group(const vector<vector<float>>& feat_list, double radius, double thresh = 0.1, int max_iter = 20)
{
    if(feat_list.size() <= 1)
        return;
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> dist(0, feat_list.size()-1);

    vector<vector<float>> ctr_list;

    int ddd = 0;
    while(ddd++ < max_iter)
    {
    int idx = dist(rd);
    vector<float> ctr(feat_list[idx]);
    int iter = 0;
    while(iter++ < max_iter)
    {
        vector<vector<float>> neighbors;
        for(const auto& f : feat_list)
        {
            float v = compare_feature(&ctr[0], &f[0], ctr.size());
            if(v < radius)
                neighbors.push_back(f);
        }
        if(neighbors.size() <= 1)
            break;
        vector<double> sum(ctr.size(), 0);
        for(const auto& f : neighbors)
        {
            for(size_t i = 0; i < f.size(); ++i)
                sum[i] += f[i];
        }
        vector<float> ctr_(ctr.size());
        for(size_t i = 0; i < ctr.size(); ++i)
            ctr_[i] = (float)(sum[i] / neighbors.size());

        // double d = 0;
        // for(size_t i = 0; i < ctr.size(); ++i)
        // {
        //     d += (ctr[i] - ctr_[i]) * (ctr[i] - ctr_[i]);
        // }
        double d = compare_feature(&ctr[0], &ctr_[0], ctr.size());
        // cout << feat_list.size() << "->" << neighbors.size() << ": " << d << endl;
        ctr = ctr_;
            
        if(d < 1.0e-4)
        {
            break;
        }
    }
    bool ok = false;
    for(const auto& c : ctr_list)
    {
        float d = compare_feature(&c[0], &ctr[0], ctr.size());
        if(d < 0.001)
        {
            ok = true;
            break;
        }
    }
    if(!ok)
        ctr_list.push_back(ctr);
    }

    for(auto it = ctr_list.begin(); it != ctr_list.end(); ++it)
    {
        for(auto jt = std::next(it); jt != ctr_list.end(); ++jt)
        {
            const auto& f0 = *it;
            const auto& f1 = *jt;
            cout << compare_feature(&f0[0], &f1[0], f0.size()) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int main(int argc, char* argv[])
{
        if(argc != 2)
    {
        cerr << "USAGE: app path" << endl;
        return 0; 
    }

    string path = argv[1];
    vector<string> filelist;
    read_directory(path, filelist);

    for(const auto& s : filelist)
    {
        vector<vector<float>> feat_list;
        if(!load_features(path + "/" + s, feat_list))
        {
            cerr << "fail to load " << s << endl;
            continue;
        }
        cout << s << endl;
        if(feat_list.size() <= 1)
            continue;

        compact_group(feat_list, 0.015);
    }

}
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iomanip>
#include <cmath>
#include <random>
#include <vector>
#include <regex>
#include <limits>
#include <algorithm>
#include <utility>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

void read_directory(const std::string &name, std::vector<std::string> &v)
{
    DIR *dirp = opendir(name.c_str());
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        if (strcmp(".", dp->d_name) == 0 || strcmp("..", dp->d_name) == 0)
            continue;
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

uint32_t numbits_lookup_table[256] = {

    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2,

    3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3,

    3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3,

    4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4,

    3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,

    6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,

    4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,

    6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5,

    3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3,

    4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6,

    6, 7, 6, 7, 7, 8

};

uint32_t compare_feature(const uint8_t *src, const uint8_t *dst, uint32_t len)
{
    uint32_t hamming = 0;
    for (uint32_t i = 0; i < len; ++i)
    {
        uint8_t v = src[i] ^ dst[i];
        hamming += numbits_lookup_table[v];
    }

    return hamming;
}

float compare_feature(const float *src, const float *dst, uint32_t len)
{
    double ms = 0, md = 0;
    double ssums = 0, ssumd = 0;
    double vars = 0, vard = 0;
    for (uint32_t i = 0; i < len; ++i)
    {
        ms += src[i];
        md += dst[i];
        ssums += src[i] * src[i];
        ssumd += dst[i] * dst[i];
    }
    ms /= len;
    md /= len;
    vars = (ssums / len) - ms * ms;
    vard = (ssumd / len) - md * md;
    double l2 = 0;
    double w = 1.0 / len / (sqrt(vars * vard)); //std::max(src[i], dst[i]);
    for (uint32_t i = 0; i < len; ++i)
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

bool load_features(const string &path, vector<vector<uint8_t>> &feature_list)
{
    ifstream ifs(path);
    if (!ifs.is_open())
        return false;

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

        vector<uint8_t> feat;
        for (auto &s : out)
        {
            if (s.empty())
                continue;
            int v = stoi(s);
            feat.push_back((uint8_t)v);
        }

        if (feat.size() != 256)
        {
            cerr << "parse bad format feature" << endl;
            continue;
        }

        feature_list.push_back(feat);
    }

    return feature_list.size() > 0;
}

bool load_features(const string &path, vector<vector<float>> &feature_list)
{
    ifstream ifs(path);
    if (!ifs.is_open())
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
        for (auto &s : out)
        {
            if (s.empty())
                continue;
            float v = stof(s);
            feat.push_back(v);
        }
        // cout << feat.size() << endl;
        if (last_feat_len != 0 && feat.size() != last_feat_len)
        {
            cerr << "bad format feature" << endl;
            continue;
        }
        last_feat_len = feat.size();

        feature_list.push_back(feat);
    }

    return feature_list.size() > 0;
}

template <typename T>
void compact_features(const vector<vector<T>> &feat_list, vector<vector<T>> &compact_list, double threshold = 550)
{
    list<vector<vector<T>>> table;
    for (const auto &feat : feat_list)
    {
        double min_dist = numeric_limits<double>::max();
        auto mit = table.begin();
        for (auto it = table.begin(); it != table.end(); ++it)
        {
            double mmin = numeric_limits<double>::max();
            for (const auto &f : *it)
            {
                auto v = compare_feature(&feat[0], &f[0], feat.size());
                if (v < mmin)
                    mmin = v;
            }
            if (mmin < min_dist)
            {
                min_dist = mmin;
                mit = it;
            }
        }
        cout << " " << min_dist;
        if (min_dist < threshold)
        {
            mit->push_back(feat);
        }
        else
        {
            vector<vector<T>> l;
            l.push_back(feat);
            table.push_back(l);
        }
    }
    cout << endl;
    cout << "table " << feat_list.size() << "->" << table.size() << endl;

    double min_dist = numeric_limits<double>::max();
    auto mit = table.begin();
    bool ok = false;
    for (auto tit = table.begin(); tit != table.end(); ++tit)
    {
        if (tit->size() <= 1)
            continue;
        double sum = 0;
        int n = 0;
        for (auto lit = tit->begin(); lit != tit->end(); ++lit)
        {
            const vector<T> &f0 = *lit;
            for (auto nit = std::next(lit); nit != tit->end(); ++nit)
            {
                const vector<T> &f1 = *nit;
                sum += compare_feature(&f0[0], &f1[0], f0.size());
                ++n;
                ok = true;
            }
        }
        double mean = sum / n;
        if (mean < min_dist)
        {
            min_dist = mean;
            mit = tit;
        }
    }
    if (ok)
    {
        // cout << "compact: " << min_dist << " " << mit->size() << endl;
        std::copy(mit->begin(), mit->end(), back_inserter(compact_list));
    }
}

void compact_group(const vector<vector<float>> &feat_list, vector<vector<float>> &compact_list, double radius, double thresh = 0.1, int max_iter = 10)
{
    if (feat_list.size() <= 1)
        return;

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> dist(0, feat_list.size() - 1);

    vector<vector<float>> neighbors;
    int idx = dist(rd);
    vector<float> ctr(feat_list[idx]);
    int iter = 0;
    while (iter++ < max_iter)
    {
        vector<vector<float>> neighbors_;
        for (const auto &f : feat_list)
        {
            float v = compare_feature(&ctr[0], &f[0], ctr.size());
            if (v < radius * 1.5)
                neighbors_.push_back(f);
        }
        if (neighbors_.size() <= 1)
            break;
        vector<double> sum(ctr.size(), 0);
        for (const auto &f : neighbors_)
        {
            for (size_t i = 0; i < f.size(); ++i)
                sum[i] += f[i];
        }
        vector<float> ctr_(ctr.size());
        for (size_t i = 0; i < ctr.size(); ++i)
            ctr_[i] = (float)(sum[i] / neighbors_.size());

        float d = compare_feature(&ctr[0], &ctr_[0], ctr.size());
        ctr = ctr_;
        if (d < 1.0e-4)
        {
            neighbors = neighbors_;
            break;
        }
    }

    for (const auto &f : neighbors)
    {
        float d = compare_feature(&ctr[0], &f[0], ctr.size());
        if (d < radius)
            compact_list.push_back(f);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "USAGE: app path" << endl;
        return 0;
    }

    string path = argv[1];
    vector<string> filelist;
    read_directory(path, filelist);

    vector<float> score;
    vector<float> coscore;

    for (auto it = filelist.begin(); it != filelist.end(); ++it)
    {
        vector<vector<float>> feat_list1;
        string s1 = *it;
        if (!load_features(path + "/" + s1, feat_list1))
        {
            // cerr << "fail to load " << s1 << endl;
            continue;
        }
        if (feat_list1.size() == 0)
            continue;

        for (auto fit = feat_list1.begin(); fit != feat_list1.end(); ++fit)
            for (auto nfit = std::next(fit); nfit != feat_list1.end(); ++nfit)
            {
                auto &f1 = *fit;
                auto &f2 = *nfit;
                float m = compare_feature(&f1[0], &f2[0], f1.size());
                score.push_back(m);
            }
        // stringstream ss;
        // ss << s1 << "(" << feat_list1.size() << "):";
        // while (ss.str().size() < 24)
        //     ss << " ";
        // ss << endl;
        // std::sort(score.begin(), score.end(), std::less<float>());
        // for (auto v : score)
        // {
        //     ss << setw(8) << setfill(' ') << v << " ";
        // }

        float minv = numeric_limits<float>::max();
        float maxv = 0;
        double sum = 0;
        int n = 0;
        int hist[20] = {0};

        for (auto nit = filelist.begin(); nit != filelist.end(); ++nit)
        {
            if (it == nit)
                continue;
            vector<vector<float>> feat_list2;
            string s2 = *nit;
            if (!load_features(path + "/" + s2, feat_list2))
            {
                // cerr << "fail to load " << s2 << endl;
                continue;
            }
            if (feat_list1.size() == 0 || feat_list2.size() == 0)
                continue;

            for (const auto &f1 : feat_list1)
                for (const auto &f2 : feat_list2)
                {
                    float m = compare_feature(&f1[0], &f2[0], f1.size());
                    coscore.push_back(m);
                    int b = (int)((m - 0.02) * 1000);
                    if (b < 0)
                        b = 0;
                    if (b > 19)
                        b = 19;
                    hist[b]++;
                    if (m < minv)
                        minv = m;
                    if (m > maxv)
                        maxv = m;
                    sum += m;
                    n++;
                }
            // for (int i = 1; i < 20; ++i)
            // {
            //     hist[i] += hist[i - 1];
            // }
        }

        // for (int i = 0; i < 20; ++i)
        // {
        //     ss << setw(3) << setfill(' ') << 100 * hist[i] / n << " ";
        // }
        // ss << endl << setw(8) << setfill(' ') << minv << "\t" << maxv << "\t" << sum / n;
        // cout << ss.str() << endl;
    }

    std::sort(score.begin(), score.end(), std::less<float>());
    std::sort(coscore.begin(), coscore.end(), std::less<float>());
    
    {
        float minv = numeric_limits<float>::max();
        float maxv = 0;
        double sum = 0;
        for (auto v : score)
        {
            if (minv > v)
                minv = v;
            if (maxv < v)
                maxv = v;
            sum += v;
        }
        int mid = score.size() / 2;
        cout << minv << "\t" << sum / score.size() << "\t" << score[mid] << "\t" << maxv << endl;
    }

    {
        float minv = numeric_limits<float>::max();
        float maxv = 0;
        double sum = 0;
        for (auto v : coscore)
        {
            if (minv > v)
                minv = v;
            if (maxv < v)
                maxv = v;
            sum += v;
        }
        int idx = coscore.size() * 0.01;
        cout << setw(10) << setfill(' ') << minv << "\t" << sum / coscore.size() << "\t" << coscore[idx] << "\t" << maxv << endl;
    }

    return 0;
}
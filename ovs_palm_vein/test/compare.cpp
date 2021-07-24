#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <regex>
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

uint32_t compare_feature(const uint8_t* src, const uint8_t* dst, uint32_t len)
{
    uint32_t hamming = 0;
    for(uint32_t i = 0; i < len; ++i)
    {
        uint8_t v = src[i] ^ dst[i];
        hamming += numbits_lookup_table[v];
    }

    return hamming;
}

bool load_features(const string& path, list<vector<uint8_t>>& feature_list)
{
    ifstream ifs(path);
    if(!ifs.is_open())
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
        for(auto& s : out)
        {
            if(s.empty())
                continue;
            int v = stoi(s);
            feat.push_back((uint8_t)v);
        }

        if(feat.size() != 256)
        {
            cerr << "parse bad format feature" << endl;
            continue;
        }

        feature_list.push_back(feat);
    }

    return feature_list.size() > 0;
}

void compact_features(const list<vector<uint8_t>>& feat_list, list<vector<uint8_t>>& compact_list, uint32_t threshold=550)
{
    list<list<vector<uint8_t>>> table;
    for(const auto& feat : feat_list)
    {
        uint32_t min_dist = numeric_limits<uint32_t>::max();
        auto mit = table.begin();
        for(auto it = table.begin(); it != table.end(); ++it)
        {
            uint32_t mmin = numeric_limits<uint32_t>::max();
            for(const auto& f : *it)
            {
                auto v = compare_feature(&feat[0], &f[0], 256);
                if(v < mmin)
                    mmin = v;
            }
            if(mmin < min_dist)
            {
                min_dist = mmin;
                mit = it;
            }
        }
        if(min_dist < threshold)
        {
            mit->push_back(feat);
        }
        else
        {
            list<vector<uint8_t>> l;
            l.push_back(feat);
            table.push_back(l);
        }
    }
    // cout << "table " << table.size() << endl;

    uint32_t min_dist = numeric_limits<uint32_t>::max();
    auto mit = table.begin();
    bool ok = false;
    for(auto tit = table.begin(); tit != table.end(); ++tit)
    {
        if(tit->size() <= 1)
            continue;
        double sum = 0;
        int n = 0;
        for(auto lit = tit->begin(); lit != tit->end(); ++lit)
        {
            const vector<uint8_t>& f0 = *lit;
            for(auto nit = std::next(lit); nit != tit->end(); ++nit)
            {
                const vector<uint8_t>& f1 = *nit;
                sum += compare_feature(&f0[0], &f1[0], 256);
                ++n;
                ok = true;
            }
        }
        double mean = sum / n;
        if(mean < min_dist)
        {
            min_dist = mean;
            mit = tit;
        }
    }
    if(ok)
    {
        // cout << "compact: " << min_dist << " " << mit->size() << endl;
        std::copy(mit->begin(), mit->end(), back_inserter(compact_list));
    }
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
    for(const auto& s1 : filelist)
    for(const auto& s2 : filelist)
    {
        list<vector<uint8_t>> feat_list1, feat_list2;
        if(!load_features(path + "/" + s1, feat_list1))
        {
            // cerr << "fail to load " << s1 << endl;
            continue;
        }
        if(!load_features(path + "/" + s2, feat_list2))
        {
            // cerr << "fail to load " << s2 << endl;
            continue;
        }

        list<vector<uint8_t>> compact_list1, compact_list2;
        compact_features(feat_list1, compact_list1, 400);
        compact_features(feat_list2, compact_list2, 400);
        if(feat_list1.size() == 0 || feat_list2.size() == 0)
            continue;
        // cout << compact_list1.size() << endl;
        // cout << compact_list2.size() << endl;
        uint32_t minv = numeric_limits<uint32_t>::max();
        uint32_t maxv = 0;
        double sum = 0;
        int n = 0;
        for(const auto& f1 : compact_list1)
        for(const auto& f2 : compact_list2)
        {
            uint32_t m = compare_feature(&f1[0], &f2[0], 256);
            if(m < minv) minv = m;
            if(m > maxv) maxv = m;
            sum += m;
            n++;
        }
        cout << s1 << " vs. " << s2 << ": ";
        cout << minv << " " << maxv << " " << sum / n << endl;
    }

    return 0;
}
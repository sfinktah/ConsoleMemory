#pragma once

#include <vector>

struct PatternByte
{
    byte byte;
    bool ignore;
};

struct AOBScanInfo
{
    std::vector<PatternByte> patternArray;

    AOBScanInfo(std::string pattern);

    std::string ToString();
};


#pragma once

#include <vector>

struct PatternByte
{
    byte byte;
    bool ignore;
};

struct AOBScanInfo
{
    std::vector<PatternByte> patternArr;

    AOBScanInfo(std::string pattern);

    std::string AOBScanInfo::tostring();
};


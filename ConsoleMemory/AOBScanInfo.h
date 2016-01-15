#pragma once

#include <vector>

struct PatternByte
{
    byte byte;
    bool ignore;
};

typedef std::vector<PatternByte> PatternByteVector;

struct AOBScanInfo
{
    PatternByteVector patternList;

    AOBScanInfo(std::string pattern);

    std::string ToString();
};


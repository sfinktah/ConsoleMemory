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
    int alignment;
    PatternByteVector patternList;

    AOBScanInfo(std::string pattern, int alignment = 4);

    std::string ToString();

    operator std::string();
};


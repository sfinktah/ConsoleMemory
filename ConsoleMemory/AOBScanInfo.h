#pragma once

#include <Windows.h>
#include <vector>

struct PatternByte
{
    byte byte;
    bool ignore;
};

typedef std::vector<PatternByte> PatternByteVector;

struct AOBScanInfo
{
    int byteAlignment;
    PatternByteVector patternList;

    AOBScanInfo(std::string pattern);
    AOBScanInfo(std::string pattern, int byteAlignment);

    std::string ToString();

    operator std::string();
};


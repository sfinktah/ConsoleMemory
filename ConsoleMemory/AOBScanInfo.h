#pragma once

#include <sstream>
#include <vector>

struct PatternByte
{
    BYTE byte;
    BOOL ignore;
};

struct PatternMask
{
    std::string pattern;
    std::string mask;
};

struct AOBScanInfo
{
    std::vector<PatternByte> patternArr;

    AOBScanInfo(std::string pattern);
    AOBScanInfo(char* pattern);

    AOBScanInfo(char* pattern, char* mask);

    std::string tostring();

    PatternMask tomask();
};


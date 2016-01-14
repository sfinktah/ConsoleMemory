#pragma once

#include <sstream>
#include <vector>

struct PatternByte
{
    BYTE byte;
    BOOL ignore;
};

//struct PatternMask
//{
//    std::string pattern;
//    std::string mask;
//
//    operator std::string()
//    {
//        return pattern + " " + mask;
//    }
//};

struct AOBScanInfo
{
    std::vector<PatternByte> patternArr;

    AOBScanInfo(std::string pattern);
    AOBScanInfo(char* pattern);

    //AOBScanInfo(char* pattern, char* mask);

    //template <size_t S> AOBScanInfo(char(pattern)[S], char(mask)[S]) : AOBScanInfo(PCHAR(pattern), PCHAR(mask))
    //{

    //}

    std::string AOBScanInfo::tostring();

    //PatternMask tomask();
};


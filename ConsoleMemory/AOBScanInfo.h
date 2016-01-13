#pragma once

#include <sstream>
#include <vector>

struct PatternByte
{
    BYTE byte;
    BOOL ignore;
};

struct AOBScanInfo
{
    std::vector<PatternByte> patternArr;

    AOBScanInfo(std::string pattern)
    {
        std::stringstream ss;
        ss << pattern;

        std::string sByte;

        while (ss >> sByte)
        {
            if (sByte == "?" || sByte == "??")
            {
                patternArr.push_back({NULL, TRUE});
            }
            else
            {
                int iByte = stoi(sByte, nullptr, 16);
                assert(iByte >= 0 && iByte <= 255);
                patternArr.push_back({BYTE(iByte), FALSE});
            }
        }
    }

    template <size_t S>
    AOBScanInfo(const char(&pattern)[S]) : AOBScanInfo(std::string(pattern))
    {
        
    }

    AOBScanInfo(char* pattern) : AOBScanInfo(std::string(pattern))
    {
        
    }

    std::string tostring()
    {
        std::stringstream ss;

        for each (PatternByte pattern in patternArr)
        {
            if (pattern.ignore)
            {
                ss << "? ";
            }
            else
            {
                ss << std::uppercase << std::hex << int(pattern.byte) << " ";
            }
        }
        return ss.str();
    }
};


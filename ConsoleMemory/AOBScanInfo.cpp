#include "stdafx.h"

#include <sstream>
#include <iomanip>

#include "AOBScanInfo.h"

AOBScanInfo::AOBScanInfo(std::string pattern)
{
    std::stringstream ss;
    ss << pattern;

    std::string sByte;

    while (ss >> sByte)
    {
        if (sByte == "?" || sByte == "??")
        {
            patternArray.push_back({ NULL, TRUE });
        }
        else
        {
            int iByte = stoi(sByte, nullptr, 16);
            BrickAssert(iByte >= 0 && iByte <= 255, "Byte %i is not between 0 and 255", iByte);
            patternArray.push_back({ BYTE(iByte), FALSE });
        }
    }
}

std::string AOBScanInfo::ToString()
{
    std::stringstream ss;

    for (PatternByte pattern : patternArray)
    {
        if (pattern.ignore)
        {
            ss << "? ";
        }
        else
        {
            ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(pattern.byte) << " ";
        }
    }
    return ss.str();
}

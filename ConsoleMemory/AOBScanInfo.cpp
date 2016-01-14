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
            patternArr.push_back({ NULL, TRUE });
        }
        else
        {
            int iByte = stoi(sByte, nullptr, 16);
            BrickAssert(iByte >= 0 && iByte <= 255, "Byte %i is not between 0 and 255", iByte);
            patternArr.push_back({ BYTE(iByte), FALSE });
        }
    }
}

std::string AOBScanInfo::tostring()
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
            ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(pattern.byte) << " ";
        }
    }
    return ss.str();
}

//PatternMask AOBScanInfo::tomask()
//{
//    std::stringstream patternStream;
//    std::stringstream maskStream;
//
//    for each (PatternByte pattern in patternArr)
//    {
//        patternStream << "\\x";
//        if (pattern.ignore)
//        {
//            patternStream << "00";
//            maskStream << "?";
//        }
//        else
//        {
//            patternStream << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(pattern.byte);
//            maskStream << "x";
//        }
//    }
//    return{ patternStream.str(), maskStream.str() };
//}

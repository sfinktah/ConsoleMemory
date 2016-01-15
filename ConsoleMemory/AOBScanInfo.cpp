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
        if ((sByte == "?") || (sByte == "??"))
        {
            patternList.push_back({ 0, true });
        }
        else
        {
            int iByte = stoi(sByte, nullptr, 16);

            BrickAssert((iByte >= 0x00) && (iByte <= 0xFF), "Byte %X is not between 0 and 255", iByte);

            patternList.push_back({ byte(iByte), false });
        }
    }
}

std::string AOBScanInfo::ToString()
{
    std::stringstream ss;

    for (PatternByte pattern : patternList)
    {
        if (pattern.ignore)
        {
            ss << "?";
        }
        else
        {
            ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(pattern.byte);
        }

        ss << " ";
    }

    return ss.str();
}

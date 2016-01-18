#include "stdafx.h"

#include "AOBScanInfo.h"

#include <sstream>
#include <iomanip>

AOBScanInfo::AOBScanInfo(std::string pattern, int alignment) : alignment(alignment)
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

            assert((iByte >= 0x00) && (iByte <= 0xFF));

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
            ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(pattern.byte); // Cast to int so it is not treated as a regular char
        }

        ss << " ";
    }

    return ss.str();
}

AOBScanInfo::operator std::string()
{
    return ToString();
}

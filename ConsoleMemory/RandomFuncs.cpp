#include "stdafx.h"
#include "RandomFuncs.h"

#include <time.h>

std::string FormatDate(std::string format)
{
    char dateBuffer[128];

    time_t timet = time(nullptr);
    tm tm;
    localtime_s(&tm, &timet);

    strftime(dateBuffer, sizeof(dateBuffer), format.c_str(), &tm);
    
    return std::string(dateBuffer);
}

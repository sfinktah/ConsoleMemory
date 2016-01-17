// stdafx.cpp : source file that includes just the standard includes
// ConsoleTestApp.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void _BrickAssert(BOOL condition, PCHAR expression, PCHAR file, PCHAR functionName, INT line, PCHAR errorMsg, ...)
{
    if (!condition)
    {
        va_list list;
        va_start(list, errorMsg);

        char buffer[1024];

        sprintf_s(buffer, errorMsg, list);

#if defined _DEBUG
        Log("[Assertion Failed] %s '%s' | %s line %u at '%s'", buffer, expression, file, line, functionName);
#else
        Log("[Assertion Failed] %s", buffer);
#endif

        va_end(list);

//#if defined _DEBUG
//        abort();
//#endif
    }
}

#pragma once

#include <TlHelp32.h>

namespace ProcessFinder
{
    PROCESSENTRY32 GetProcessFromName(wchar_t* processName);

    MODULEENTRY32 GetProcessModule(DWORD processID, wchar_t* moduleName);

    MODULEENTRY32 GetMainModule(DWORD processID);
};


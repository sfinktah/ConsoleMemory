#pragma once

#include <TlHelp32.h>
#include <string>

namespace ProcessFinder
{
    PROCESSENTRY32 GetProcessFromName(std::wstring processName);

    MODULEENTRY32 GetProcessModule(DWORD processID, std::wstring moduleName);

    MODULEENTRY32 GetMainModule(DWORD processID);
};


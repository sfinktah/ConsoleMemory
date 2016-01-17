#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>

#include "RemotePointer.h"

namespace ProcessFinder
{
    PROCESSENTRY32 GetProcessFromName(std::wstring processName);

    MODULEENTRY32 GetProcessModule(DWORD processID, std::wstring moduleName);

    MODULEENTRY32 GetMainModule(DWORD processID);

    MODULEENTRY32 GetAddressInfo(RPtr rPtr, uintptr_t address);
};


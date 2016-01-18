#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include <string>

namespace ProcessFinder
{
    PROCESSENTRY32 GetProcessFromName(std::wstring processName);

    MODULEENTRY32 GetProcessModule(DWORD pID, std::wstring moduleName);

    MODULEENTRY32 GetMainModule(DWORD pID);

    MODULEENTRY32 GetAddressInfo(DWORD pID, uintptr_t address);

    MODULEINFO GetModuleInfo(HANDLE hProcess, HMODULE hModule);
};


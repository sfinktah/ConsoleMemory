#include "stdafx.h"
#include "ProcessFinder.h"

namespace ProcessFinder
{
    PROCESSENTRY32 GetProcessFromName(wchar_t* processName)
    {
        HANDLE hProcSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        PROCESSENTRY32 processEntry = { };
        processEntry.dwSize = sizeof(processEntry);

        BrickAssert(Process32First(hProcSnapshot, &processEntry), "Could not get first process");

        do
        {
            if (wcscmp(processEntry.szExeFile, processName) == 0)
            {
                CloseHandle(hProcSnapshot);

                return processEntry;
            }
        }
        while (Process32Next(hProcSnapshot, &processEntry));

        return { };
    }

    MODULEENTRY32 GetProcessModule(DWORD pID, wchar_t* moduleName)
    {
        HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

        MODULEENTRY32 moduleEntry = { };
        moduleEntry.dwSize = sizeof(moduleEntry);

        BrickAssert(Module32First(hModSnapshot, &moduleEntry), "Could not get first process module");

        do
        {
            if (wcsstr(moduleEntry.szExePath, moduleName) != nullptr)
            {
                return moduleEntry;
            }
        }
        while (Module32Next(hModSnapshot, &moduleEntry));

        return { };
    }

    MODULEENTRY32 GetMainModule(DWORD pID)
    {
        HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

        MODULEENTRY32 moduleEntry = { };
        moduleEntry.dwSize = sizeof(moduleEntry);

        if (Module32First(hModuleSnapshot, &moduleEntry))
        {
            return moduleEntry;
        }

        return { };
    }
}


#include "stdafx.h"

#include "ProcessFinder.h"

namespace ProcessFinder
{
    PROCESSENTRY32 GetProcessFromName(std::wstring processName)
    {
        HANDLE hProcSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        PROCESSENTRY32 processEntry = { };
        processEntry.dwSize = sizeof(processEntry);

        assert(Process32First(hProcSnapshot, &processEntry));

        do
        {
            if (_wcsicmp(processName.data(), processEntry.szExeFile) == 0)
            {
                CloseHandle(hProcSnapshot);

                return processEntry;
            }
        } while (Process32Next(hProcSnapshot, &processEntry));

        CloseHandle(hProcSnapshot);

        return { };
    }

    MODULEENTRY32 GetProcessModule(DWORD pID, std::wstring moduleName)
    {
        HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

        MODULEENTRY32 moduleEntry = { };
        moduleEntry.dwSize = sizeof(moduleEntry);

        assert(Module32First(hModSnapshot, &moduleEntry));

        do
        {
            if (_wcsicmp(moduleName.data(), moduleEntry.szModule) == 0)
            {
                CloseHandle(hModSnapshot);

                return moduleEntry;
            }
        } while (Module32Next(hModSnapshot, &moduleEntry));

        CloseHandle(hModSnapshot);

        return { };
    }

    MODULEENTRY32 GetMainModule(DWORD pID)
    {
        HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

        MODULEENTRY32 moduleEntry = { };
        moduleEntry.dwSize = sizeof(moduleEntry);

        if (Module32First(hModSnapshot, &moduleEntry))
        {
            CloseHandle(hModSnapshot);

            return moduleEntry;
        }

        Log("Could not get main module of process %u", pID);

        CloseHandle(hModSnapshot);

        return { };
    }

    MODULEENTRY32 GetAddressInfo(RPtr rPtr, uintptr_t address)
    {
        HANDLE pHandle = rPtr.handle;

        if (pHandle)
        {
            DWORD pID = GetProcessId(pHandle);

            HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

            MODULEENTRY32 moduleEntry = { };
            moduleEntry.dwSize = sizeof(moduleEntry);

            assert(Module32First(hModSnapshot, &moduleEntry));

            do
            {
                uintptr_t moduleAddress = uintptr_t(moduleEntry.modBaseAddr);
                LogDebug("[GetAddresInfo] Checking 0x%I64X", moduleAddress);

                if ((moduleAddress < address) && ((moduleAddress + moduleEntry.modBaseSize) > address))
                {
                    //CloseHandle(hModSnapshot);

                    return moduleEntry;
                }

            } while (Module32Next(hModSnapshot, &moduleEntry));

            //CloseHandle(hModSnapshot);

            Log("[GetAddresInfo] Could not get info of 0x%I64X", address);
        }


        return { };
    }
}


#include "stdafx.h"

#include "ProcessFinder.h"

/*
    TH32CS_INHERIT 		| Indicates that the snapshot handle is to be inheritable.
    TH32CS_SNAPHEAPLIST | Includes all heaps of the process specified in th32ProcessID in the snapshot. To enumerate the heaps, see Heap32ListFirst.
    TH32CS_SNAPMODULE 	| Includes all modules of the process specified in th32ProcessID in the snapshot. To enumerate the modules, see Module32First.
    TH32CS_SNAPMODULE32 | Includes all 32-bit modules of the process specified in th32ProcessID in the snapshot when called from a 64-bit process.
    NOTE: To Snapshot both 32 and 64 bit modules in a 64 bit process, use both flags. (TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32)    
    TH32CS_SNAPPROCESS 	| Includes all processes in the system in the snapshot. To enumerate the processes, see Process32First.
    TH32CS_SNAPTHREAD 	| Includes all threads in the system in the snapshot. To enumerate the threads, see Thread32First.
*/

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

    MODULEENTRY32 GetAddressInfo(DWORD pID, uintptr_t address)
    {
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
                CloseHandle(hModSnapshot);

                return moduleEntry;
            }

        } while (Module32Next(hModSnapshot, &moduleEntry));

        CloseHandle(hModSnapshot);

        Log("[GetAddresInfo] Could not get info of 0x%I64X", address);

        return { };
    }

    MODULEINFO GetModuleInfo(HANDLE hProcess, HMODULE hModule)
    {
        MODULEINFO moduleInfo;

        BOOL success = K32GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo));

        assert(success);

        return moduleInfo;
    }
}


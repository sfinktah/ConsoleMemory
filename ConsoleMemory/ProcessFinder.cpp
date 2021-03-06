#include "stdafx.h"

#include "ProcessFinder.h"

/*
    TH32CS_INHERIT 		| Indicates that the snapshot handle is to be inheritable.

    TH32CS_SNAPHEAPLIST | Includes all heaps of the process specified in th32ProcessID in the snapshot.
    To enumerate the heaps, see Heap32ListFirst.
    
    TH32CS_SNAPMODULE 	| Includes all modules of the process specified in th32ProcessID in the snapshot.
    To enumerate the modules, see Module32First.
    
    TH32CS_SNAPMODULE32 | Includes all 32-bit modules of the process specified in th32ProcessID in the snapshot when called from a 64-bit process.
    
    NOTE: To Snapshot both 32 and 64 bit modules in a 64 bit process, use both flags. (TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32) 

    TH32CS_SNAPPROCESS 	| Includes all processes in the system in the snapshot. 
    To enumerate the processes, see Process32First.

    TH32CS_SNAPTHREAD 	| Includes all threads in the system in the snapshot. 
    To enumerate the threads, see Thread32First.
*/

PROCESSENTRY32 GetProcessFromName(std::wstring processName)
{
    HANDLE hProcSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    PROCESSENTRY32 processEntry = { };
    processEntry.dwSize = sizeof(processEntry); // Have to set dwSize else Process32First fails

    if (Process32First(hProcSnapshot, &processEntry))
    {

        do
        {
            if (_wcsicmp(processName.c_str(), processEntry.szExeFile) == 0) // If the strings are equal (ignoring case)
            {
                CloseHandle(hProcSnapshot);

                Log("Found process %s. Process ID: %u", std::string(processName.begin(), processName.end()).c_str(), processEntry.th32ProcessID);

                return processEntry;
            }
        } while (Process32Next(hProcSnapshot, &processEntry));
    }

    CloseHandle(hProcSnapshot);

    Log("Failed to find process %s", std::string(processName.begin(), processName.end()).c_str());

    return { };
}

MODULEENTRY32 GetProcessModule(DWORD pID, std::wstring moduleName)
{
    HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

    MODULEENTRY32 moduleEntry = { };
    moduleEntry.dwSize = sizeof(moduleEntry); // Have to set dwSize else Module32First fails

    if (Module32First(hModSnapshot, &moduleEntry))
    {
        do
        {
            if (_wcsicmp(moduleName.c_str(), moduleEntry.szModule) == 0) // If the strings are equal (ignoring case)
            {
                CloseHandle(hModSnapshot);

                Log("Found module %s in process %u. Base Address: 0x%I64X, Size: 0x%X", std::string(moduleName.begin(), moduleName.end()).c_str(), pID, uintptr_t(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);

                return moduleEntry;
            }
        } while (Module32Next(hModSnapshot, &moduleEntry));
    }

    CloseHandle(hModSnapshot);

    Log("Failed to find module %s in process %u", std::string(moduleName.begin(), moduleName.end()).c_str(), pID);

    return { };
}

MODULEENTRY32 GetMainModule(DWORD pID)
{
    HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

    MODULEENTRY32 moduleEntry = { };
    moduleEntry.dwSize = sizeof(moduleEntry); // Have to set dwSize else Module32First fails

    if (Module32First(hModSnapshot, &moduleEntry)) // Module32First returns the main module of the process (if successfull)
    {
        CloseHandle(hModSnapshot);

        Log("Found main module of process %u. Base Address: 0x%I64X, Size: 0x%X", pID, uintptr_t(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);

        return moduleEntry;
    }

    CloseHandle(hModSnapshot);

    Log("Failed to get main module of process %u", pID);

    return { };
}

MODULEENTRY32 GetAddressInfo(DWORD pID, uintptr_t address)
{
    HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

    MODULEENTRY32 moduleEntry = { };
    moduleEntry.dwSize = sizeof(moduleEntry); // Have to set dwSize else Module32First fails

    if (Module32First(hModSnapshot, &moduleEntry))
    {
        do
        {
            uintptr_t moduleBegin = uintptr_t(moduleEntry.modBaseAddr);
            uintptr_t moduleEnd = moduleBegin + moduleEntry.modBaseSize;

            if ((address > moduleBegin) && (address < moduleEnd)) // If the address is between the start and end of the module
            {
                CloseHandle(hModSnapshot);

                Log("Found info for address at 0x%I64X in process 0x%X", address, pID);

                return moduleEntry;
            }
        } while (Module32Next(hModSnapshot, &moduleEntry));
    }

    CloseHandle(hModSnapshot);

    Log("Failed to find info for address at 0x%I64X in process 0x%X", address, pID);

    return { };
}

MODULEINFO GetModuleInfo(HANDLE hProcess, HMODULE hModule)
{
    MODULEINFO moduleInfo;

    BOOL success = GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo));

    BrickAssert(success);

    return moduleInfo;
}


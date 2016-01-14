// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MemDump.h"
#include "ProcessFinder.h"

int main()
{
    PROCESSENTRY32 processEntry = ProcessFinder::GetProcessFromName(L"GTA5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL, "Could not find Process");

    //while (!processEntry.th32ProcessID)
    //{
    //    Sleep(5000);

    //    Log("Searching for GTA5.exe");

    //    processEntry = ProcessFinder::GetProcessFromName(L"GTA5.exe");
    //}

    //Log("Found GTA5.exe");

    MODULEENTRY32 processModule = ProcessFinder::GetMainModule(processEntry.th32ProcessID);
    BrickAssert(processModule.dwSize != NULL, "Could not get main module");

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr, "Could not get process handle");

    RPtr ptr(pHandle);

    MemDump memDump = MemDump(ptr);

    memDump.Scan();

    memDump.Print(); 

    AOBScanInfo tunableScan = "48 8B 8C C2 ? ? ? ? 48 85 C9 74 19";

    uintptr_t aobResult = memDump.AOBScan(tunableScan);

    uintptr_t aobPtr = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr + ptr.Read<int>(aobResult + 4) + 8));

    Log("Tunables pointer 0x%I64X", aobPtr);

    memDump.Free();

    CloseHandle(pHandle);

    getchar();

    return 0;
}

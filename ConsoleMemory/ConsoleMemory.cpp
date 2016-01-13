// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>

#include "MemDump.h"
#include "ProcessFinder.h"

int main()
{
    PROCESSENTRY32 processEntry = ProcessFinder::GetProcessFromName(L"chrome.exe");
    assert(processEntry.dwSize != NULL);
    MODULEENTRY32 processModule = ProcessFinder::GetMainModule(processEntry.th32ProcessID);
    assert(processModule.dwSize != NULL);
    //MODULEENTRY32 processModule = GetProcessModule(processEntry);
    //assert(processModule.dwSize != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);

    MemDump* memDump = new MemDump(pHandle);

    memDump->Scan();

    memDump->Dump();

    RPtr tunablesAOBPtr = memDump->AOBScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");
    DWORD64 pointer = tunablesAOBPtr;

    //int offset = ReadRemoteMemory<int>(memDump.GetHandle( ), tunablesAOB + 4);
    //BYTE* tunablesPointer = ReadRemoteMemory<BYTE*>(memDump.GetHandle( ), processModule.modBaseAddr + offset + 8);

    //Log("Tunables 0x%I64X", DWORD64(tunablesPointer));

    memDump->Free();

    CloseHandle(pHandle);

    getchar();

    return 0;
}


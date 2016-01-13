// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>

#include "MemDump.h"
#include "ProcessFinder.h"

int main()
{
    PROCESSENTRY32 processEntry = ProcessFinder::GetProcessFromName(L"GTA5.exe");
    assert(processEntry.dwSize != NULL);
    MODULEENTRY32 processModule = ProcessFinder::GetMainModule(processEntry.th32ProcessID);
    assert(processModule.dwSize != NULL);
    //MODULEENTRY32 processModule = GetProcessModule(processEntry);
    //assert(processModule.dwSize != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    assert(pHandle != nullptr);

    MemDump* memDump = new MemDump(pHandle);

    memDump->Scan();

    memDump->Print();

    RPtr tunablesAOBPtr = memDump->AOBScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    //int offset = ReadRemoteMemory<int>(memDump.GetHandle( ), tunablesAOB + 4);
    //BYTE* tunablesPointer = ReadRemoteMemory<BYTE*>(memDump.GetHandle( ), processModule.modBaseAddr + offset + 8);

    RPtr tunablesPointer = RPtr(pHandle, processModule.modBaseAddr + tunablesAOBPtr.Get<int>(4) + 8, { 0 });

    Log("Tunables 0x%I64X", DWORD64(tunablesPointer));

    memDump->Free();

    CloseHandle(pHandle);

    getchar();

    return 0;
}
// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MemDump.h"
#include "ProcessFinder.h"

int main()
{
    PROCESSENTRY32 processEntry = ProcessFinder::GetProcessFromName(L"GTA5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL, "Could not find GTA5.exe");

    while (!processEntry.th32ProcessID)
    {
        Sleep(5000);

        Log("Searching for GTA5.exe");

        processEntry = ProcessFinder::GetProcessFromName(L"GTA5.exe");
    }
  
    Log("Found GTA5.exe");

    MODULEENTRY32 processModule = ProcessFinder::GetMainModule(processEntry.th32ProcessID);
    BrickAssert(processModule.dwSize != NULL, "Could not get main module");

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr, "Could not get process handle");

    MemDump* memDump = new MemDump(pHandle);

    memDump->Scan();

    //memDump->Print();

    RPtr tunablesAOBPtr = memDump->AOBScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    while (tunablesAOBPtr.pointer == NULL)
    {
        memDump->Scan();
        tunablesAOBPtr = memDump->AOBScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

        Sleep(10000);
    }

    //int offset = ReadRemoteMemory<int>(memDump.GetHandle( ), tunablesAOB + 4);
    //BYTE* tunablesPointer = ReadRemoteMemory<BYTE*>(memDump.GetHandle( ), processModule.modBaseAddr + offset + 8);

    RPtr tunablesPointer = RPtr(pHandle, processModule.modBaseAddr + tunablesAOBPtr.Get<int>(4) + 8, { 0 });

    Log("Tunables 0x%I64X", DWORD64(tunablesPointer));

    memDump->Free();

    delete memDump;

    CloseHandle(pHandle);

    getchar();

    return 0;
}
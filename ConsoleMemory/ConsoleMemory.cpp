// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MemDump.h"
#include "ProcessFinder.h"

#include <array>

#include "IniConfig.h"

void testdump()
{
    system("PAUSE");

    IniConfig config = IniConfig::FromFile("tunables.ini");

    PROCESSENTRY32 processEntry = ProcessFinder::GetProcessFromName(L"gta5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL, "Could not find Process");

    MODULEENTRY32 processModule = ProcessFinder::GetMainModule(processEntry.th32ProcessID);
    BrickAssert(processModule.dwSize != NULL, "Could not get main module");

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr, "Could not get process handle");

    RPtr ptr(pHandle);

    MemDump* memDump = new MemDump(ptr);

    memDump->Scan();
    memDump->Print();

    AOBScanInfo tunableScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    uintptr_t aobResult = memDump->AOBScan(tunableScan);

    uintptr_t aobPtr = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr + ptr.Read<int>(aobResult + 4) + 8));

    Log("Tunables pointer 0x%I64X", aobPtr);
    while (true)
    {
        for (IniValuePair valuePair : config["float"])
        {
            int index = std::stoi(valuePair.first);
            float value = std::stof(valuePair.second);
            Log("Writing tunable float %f to index %i", value, index);
            ptr.Write(aobPtr + (index * 8), value);
        }

        for (IniValuePair valuePair : config["int"])
        {
            int index = std::stoi(valuePair.first);
            int value = std::stoi(valuePair.second);
            Log("Writing tunable int %i to index %i", value, index);
            ptr.Write(aobPtr + (index * 8), value);
        }

        system("PAUSE");
    }

    memDump->Free();

    CloseHandle(pHandle);
}

void testarrayaccess()
{
    RPtr ptr(GetCurrentProcess());

    int arr[ ] { 42, 360, 420, 1337 };

    for (int i : arr)
    {
        printf_s("%i\n", i);
    }

    ptr.WriteArray<int>(uintptr_t(&arr), { 1, 2, 3, 4 });

    std::vector<int> arr2 = ptr.ReadArray<int>(uintptr_t(&arr), 4);

    for (int i : arr2)
    {
        printf_s("%i\n", i);
    }
}

void testini()
{
    IniConfig config = IniConfig::FromString("[int]\n1337 = 420\n\n[float]\n163 = 199999.0");

    for (IniValuePair valuePair : config["float"])
    {
        int index = std::stoi(valuePair.first);
        float value = std::stof(valuePair.second);
        Log("float %f to index %i", value, index);
    }

    for (IniValuePair valuePair : config["int"])
    {
        int index = std::stoi(valuePair.first);
        int value = std::stoi(valuePair.second);
        Log("int %i to index %i", value, index);
    }

    config["int"]["42"] = std::to_string(1337);

    Log("As String\n%s", config.ToString().c_str());

    config.SaveToFile("tunables.ini");
}

int main()
{

    //testarrayaccess();

    //testdump();

    testini();

    system("PAUSE");

    return 0;
}

// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MemDump.h"
#include "ProcessFinder.h"

#include <array>

#include "IniConfig.h"

void testdump()
{
    IniConfig config = IniConfig::FromFile("tunables.ini");

    PROCESSENTRY32 processEntry = ProcessFinder::GetProcessFromName(L"gta5.exe");
    assert(processEntry.th32ProcessID != NULL);

    MODULEENTRY32 processModule = ProcessFinder::GetMainModule(processEntry.th32ProcessID);
    assert(processModule.th32ProcessID != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    assert(pHandle != nullptr);

    RPtr ptr(pHandle);

    MemDump* memDump = new MemDump(ptr);

    memDump->Scan(
        //PAGE_READWRITE        |  // Needed for water
        //PAGE_EXECUTE          |
        //PAGE_EXECUTE_READ     |
        //PAGE_EXECUTE_READWRITE
            );

    memDump->Print();

    //AOBScanInfo waterScan1("1A 1A 1A 1A 00 00 00 00 00 00 00 00 00 00 00 00 04", 4); // The main ocean
    //AOBScanInfo waterScan2("1A 1A 1A 1A 00 00 00 00 00 00 00 00 C9 56 20 43 04", 4); // Land Act reservoir
    //AOBScanInfo waterScan3("1A 1A 1A 1A 00 00 00 00 00 00 00 00 00 00 F0 41 04", 4); // Alamo Sea
    //AOBScanInfo waterScan4("1A 1A 1A 1A 00 00 00 00 00 00 00 00 B4 68 44 43 04", 4); // Lake vinewood    

    //std::vector<uintptr_t> water1Result = memDump->AOBScanArray(waterScan1);
    //std::vector<uintptr_t> water2Result = memDump->AOBScanArray(waterScan2);
    //std::vector<uintptr_t> water3Result = memDump->AOBScanArray(waterScan3);
    //std::vector<uintptr_t> water4Result = memDump->AOBScanArray(waterScan4);    

    //Log("The main ocean     %I64u", water1Result.size());
    //Log("Land Act reservoir %I64u", water2Result.size());
    //Log("Alamo Sea          %I64u", water3Result.size());
    //Log("Lake vinewood      %I64u", water4Result.size());

    //for (uintptr_t waterPtr : water1Result)
    //{
    //    ptr.WriteArray<byte>(waterPtr + 12, { 0x29, 0x9C, 0x40, 0xC3, 0x04 });
    //}

    //for (uintptr_t waterPtr : water2Result)
    //{
    //    ptr.WriteArray<byte>(waterPtr + 12, { 0x00, 0x00, 0x00, 0x00, 0x04 });
    //}

    //for (uintptr_t waterPtr : water3Result)
    //{
    //    ptr.WriteArray<byte>(waterPtr + 12, { 0x00, 0x00, 0x70, 0xC1, 0x04 });
    //}

    //for (uintptr_t waterPtr : water4Result)
    //{     
    //    uintptr_t tunablesResult = memDump->AOBScan(tunableScan); 
    //    ptr.WriteArray<byte>(waterPtr + 12, { 0x00, 0x00, 0xA0, 0xC0, 0x04 });
    //}

    AOBScanInfo tunableScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    uintptr_t tunablesResult = memDump->AOBScan(tunableScan);

    uintptr_t tunablesPtr = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr) + ptr.Read<int>(tunablesResult + 4) + 8);

    MODULEENTRY32 modEntry = ProcessFinder::GetAddressInfo(ptr, tunablesResult);

    char path[MAX_PATH];
    wcstombs_s(nullptr, path, modEntry.szModule, MAX_PATH);    
    Log("%s", path);

    system("PAUSE");

    Log("Tunables pointer 0x%I64X", tunablesPtr);

    memDump->Free();

    system("PAUSE");

    while (true)
    {
        for (IniValuePair valuePair : config["float"])
        {
            int index = std::stoi(valuePair.first);
            float value = std::stof(valuePair.second);
            Log("Writing tunable %f at index %i", value, index);
            ptr.Write(tunablesPtr + (index * 8), value);
        }

        for (IniValuePair valuePair : config["int"])
        {
            int index = std::stoi(valuePair.first);
            int value = std::stoi(valuePair.second);
            Log("Writing tunable %i at index %i", value, index);
            ptr.Write(tunablesPtr + (index * 8), value);
        }

        system("PAUSE");
    }

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

    testdump();

    //testini();

    system("PAUSE");

    return 0;
}

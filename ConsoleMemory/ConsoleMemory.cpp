// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MemDump.h"
#include "IniConfig.h"
#include "ProcessFinder.h"

void testdump()
{
    IniConfig config = IniConfig::FromFile("tunables.ini");

    PROCESSENTRY32 processEntry = GetProcessFromName(L"gta5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL);

    MODULEENTRY32 processModule = GetMainModule(processEntry.th32ProcessID);
    BrickAssert(processModule.th32ProcessID != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr);

    RPtr ptr(pHandle);

    MemDump* memDump = new MemDump(ptr);

    memDump->ScanModule(processModule);

    //memDump->ScanRange(uintptr_t(processModule.modBaseAddr), processModule.modBaseSize); // Only scan the main module
    
    //memDump->Scan(PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE);

    memDump->Print();

    //AOBScanInfo waterScan1("1A 1A 1A 1A 00 00 00 00 00 00 00 00 00 00 00 00 04"); // The main ocean
    //AOBScanInfo waterScan2("1A 1A 1A 1A 00 00 00 00 00 00 00 00 C9 56 20 43 04"); // Land Act reservoir
    //AOBScanInfo waterScan3("1A 1A 1A 1A 00 00 00 00 00 00 00 00 00 00 F0 41 04"); // Alamo Sea
    //AOBScanInfo waterScan4("1A 1A 1A 1A 00 00 00 00 00 00 00 00 B4 68 44 43 04"); // Lake vinewood    

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
    //    ptr.WriteArray<byte>(waterPtr + 12, { 0x00, 0x00, 0xA0, 0xC0, 0x04 });
    //}

    AOBScanInfo tunableScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    uintptr_t tunablesResult = memDump->AOBScan(tunableScan);

    uintptr_t tunablesPtr = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr) + ptr.Read<int>(tunablesResult + 4) + 8);

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

void testptr()
{
    Log("Testing ReadPtr and WritePtr");

    const int initial = 42;
    const int after = 1337;

    RPtr ptr = RPtr::LocalPtr();

    int i = initial;

    BrickAssert(i == ptr.ReadPtr(&i));

    ptr.WritePtr(&i, after);

    BrickAssert(i == after);

    Log("ReadPtr and WritePtr test success");
}

void testarrayaccess()
{
    Log("Testing ReadArray and WriteArray");

    const int arrSize = 5;

    RPtr ptr = RPtr::LocalPtr();

    int arr[arrSize];

    for (int i = 0; i < arrSize; ++i)
    {
        arr[i] = i;
    }

    std::vector<int> read = ptr.ReadArray<int>(uintptr_t(&arr), arrSize);

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(read[i] == i);
    }

    std::vector<int> toWrite(arrSize);

    for (int i = 0; i < arrSize; ++i)
    {
        toWrite[i] = (i * 10);
    }

    ptr.WriteArray(uintptr_t(&arr), toWrite);

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(arr[i] == (i * 10));
    }

    Log("ReadArray and WriteArray test success");
}

void testini()
{
    IniConfig config = IniConfig::FromString("[int]\n1337 = 420\n\n[float]\n163 = 199999.0");

    for (IniValuePair valuePair : config["float"])
    {
        int index = std::stoi(valuePair.first);
        float value = std::stof(valuePair.second);
        Log("Float %f to index %i", value, index);
    }

    for (IniValuePair valuePair : config["int"])
    {
        int index = std::stoi(valuePair.first);
        int value = std::stoi(valuePair.second);
        Log("Int %i to index %i", value, index);
    }

    config["int"]["42"] = std::to_string(1337);

    Log("As String\n%s", config.ToString().c_str());
}

void testindexaccess()
{
    Log("Testing ReadIndex and WriteIndex");

    const int arrSize = 10;

    RPtr ptr = RPtr::LocalPtr();

    int* arr = new int[arrSize];

    for (int i = 0; i < arrSize; ++i)
    {
        arr[i] = i; // Do the intial set
    }

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(ptr.ReadIndex(arr, i) == i); // BrickAssert the ReadIndex is what it should be
    }

    for (int i = 0; i < arrSize; ++i)
    {
        ptr.WriteIndex(arr, i, arrSize - i); // Test WriteIndex
    }

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(arr[i] == (arrSize - i)); // BrickAssert the value from WriteIndex is correct
    }

    delete[ ] arr;

    Log("ReadIndex and WriteIndex test success");
}

void teststringaccess()
{
    Log("Testing ReadString and WriteString");

    RPtr ptr = RPtr::LocalPtr();

    std::string beginString = "The quick brown fox jumps over the lazy dog";
    std::string endString   = "Pack my box with five dozen liquor jugs";

    std::string currString = beginString;

    std::string readString = ptr.ReadString(uintptr_t(currString.data()), currString.length());

    BrickAssert(currString == beginString);

    ptr.WriteString(uintptr_t(beginString.data()), endString);

    BrickAssert(currString == endString);

    Log("ReadString and WriteString test sucesss")
}

int main()
{
    testdump();
    //testini();
    //testarrayaccess();    
    //testindexaccess();
    //testptr();
    //teststringaccess();

    system("PAUSE");

    return 0;
}

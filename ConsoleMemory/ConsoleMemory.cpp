// ConsoleTestApp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MemDump.h"
#include "IniConfig.h"
#include "ProcessFinder.h"

#include "UnitTests.h"
#include "TunablesDumper.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <regex>

void MainLoop()
{
    std::string fileName;
    std::cin >> fileName;

    IniConfig config = IniConfig::FromFile(fileName);

    PROCESSENTRY32 processEntry = GetProcessFromName(L"gta5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL);

    MODULEENTRY32 processModule = GetMainModule(processEntry.th32ProcessID);
    BrickAssert(processModule.th32ProcessID != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr);

    RMem ptr(pHandle);

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

    AOBScanInfo tunableScan     ("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19", 4);

    uintptr_t tunablesResult    = memDump->AOBScan(tunableScan);

    uintptr_t tunablesPtr       = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr) + ptr.Read<int>(tunablesResult + 0x4) + 0x8);
    
    Log("Tunables pointer 0x%I64X", tunablesPtr);

    //AOBScanInfo worldScan       ("48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07", 1);

    //uintptr_t worldResult       = memDump->AOBScan(worldScan);

    //uintptr_t worldPtr          = worldResult + ptr.Read<int>(worldResult + 0x3) + 0x7;

    //uintptr_t playerPtr         = ptr.Offset(worldPtr,      { 0x8 });
    //uintptr_t playerInfoPtr     = ptr.Offset(playerPtr,     { 0x1088 });

    //uintptr_t healthPtr         = ptr.Offset(playerPtr,     { 0x280 });
    //uintptr_t maxHealthPtr      = ptr.Offset(playerPtr,     { 0x284 });

    //uintptr_t namePtr           = ptr.Offset(playerInfoPtr, { 0x7C });

    //Log("World Ptr 0x%I64X", worldPtr);

    PAUSE;

    while (true)
    {
        //std::string playerName = ptr.ReadString(namePtr, 32);

        //ptr.Write<float>(healthPtr, 9999.0f);
        //ptr.Write<float>(maxHealthPtr, 9999.0f);

        //Log("Player Name: %s", playerName.c_str());

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

        PAUSE;
    }

    memDump->Free();

    CloseHandle(pHandle);
}


//struct scrNativeCallContext
//{
//    void* m_pReturn;
//    uint32_t m_nArgCount;
//    void* m_pArgs;
//
//    uint32_t m_nDataCount;
//};
//
//typedef void(__cdecl * NativeHandler)(scrNativeCallContext* context);
//
//struct NativeRegistration
//{
//    NativeRegistration* nextRegistration;
//    NativeHandler handlers[7];
//    uint32_t numEntries;
//    uint64_t hashes[7];
//};
//
//void DumpNatives()
//{
//    PROCESSENTRY32 processEntry = GetProcessFromName(L"gta5.exe");
//    BrickAssert(processEntry.th32ProcessID != NULL);
//
//    MODULEENTRY32 processModule = GetMainModule(processEntry.th32ProcessID);
//    BrickAssert(processModule.th32ProcessID != NULL);
//
//    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
//    BrickAssert(pHandle != nullptr);
//
//    uintptr_t baseAddress = uintptr_t(processModule.modBaseAddr);
//
//    RMem ptr(pHandle);
//
//    MemDump* memDump = new MemDump(ptr);
//
//    memDump->ScanModule(processModule);
//
//    AOBScanInfo tableScan("76 61 49 8B 7A 40 48 8D 0D");
//
//    uintptr_t tableResult = memDump->AOBScan(tableScan);
//
//    memDump->Free();
//
//    uintptr_t tablePtr = tableResult + ptr.Read<DWORD>(tableResult + 9) + 13;
//
//    Log("Table Ptr: %I64X", tablePtr);
//
//    char dateBuffer[80];
//    char dateBuffer2[80];
//    time_t timet = time(nullptr);
//    tm tm;
//    localtime_s(&tm, &timet);
//    strftime(dateBuffer, sizeof(dateBuffer), "%d.%m", &tm);
//    strftime(dateBuffer2, sizeof(dateBuffer2), "%R, %d.%m.%y", &tm);
//
//    std::string dateString(dateBuffer);
//    std::string dateString2(dateBuffer2);
//
//    std::ofstream outStream("NativesDump." + dateString + ".txt");
//
//    outStream << "Natives Dumped at " << dateString2 << " by Brick's Native Dumper" << std::endl << std::endl;
//
//    std::vector<NativeRegistration*> tableVec = ptr.ReadArray<NativeRegistration*>(tablePtr, 256);
//
//    for (NativeRegistration* table : tableVec)
//    {
//        NativeRegistration currTable = ptr.ReadPtr(table);
//
//        while (true)
//        {
//            for (uint32_t i = 0; i < currTable.numEntries; ++i)
//            {
//                uint64_t hash = currTable.hashes[i];
//                NativeHandler handler = currTable.handlers[i];
//
//                if ((hash) && (handler))
//                {
//                    uintptr_t localAddress = uintptr_t(handler) - baseAddress;
//
//                    outStream << "Hash: " << std::setw(16) << std::setfill('0') << std::uppercase << std::hex << hash << " | ";
//                    outStream << "Address: " << std::setw(8) << std::setfill('0') << std::uppercase << std::hex << localAddress << std::endl;
//                }
//            }
//
//            if (currTable.nextRegistration)
//            {
//                currTable = ptr.ReadPtr(currTable.nextRegistration);
//            }
//            else
//            {
//                break;
//            }
//        }
//    }
//
//    outStream.flush();
//    outStream.close();
//}

int main()
{
    //MainLoop();

    DumpTunables();

    //TestAll();

    //DumpNatives();

    system("PAUSE");

    return 0;
}

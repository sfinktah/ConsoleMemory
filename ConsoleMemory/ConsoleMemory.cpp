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

void EditTunables(std::string fileName)
{
    if (fileName.empty())
    {
        std::cout << "Enter Tunables Config Name: ";
        std::cin >> fileName;
    }

    IniConfig config = IniConfig::FromFile(fileName);

    PROCESSENTRY32 processEntry = GetProcessFromName(L"gta5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL);

    MODULEENTRY32 processModule = GetProcessModule(processEntry.th32ProcessID, L"gta5.exe");
    BrickAssert(processModule.th32ProcessID != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr);

    RMem ptr(pHandle);

    MemDump* memDump = new MemDump(ptr);

    memDump->DumpModule(processModule);

    AOBScanInfo tunableScan     ("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    uintptr_t tunablesResult    = memDump->AOBScan(tunableScan);

    uintptr_t tunablesPtr       = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr) + ptr.Read<int>(tunablesResult + 0x4) + 0x8);
    
    Log("Tunables pointer 0x%I64X", tunablesPtr);

    PAUSE;

    while (true)
    {
        for (IniValuePair valuePair : config["float"])
        {
            auto index = std::stoi(valuePair.first);
            auto value = std::stof(valuePair.second);
            Log("Writing tunable %f at index %i", value, index);
            ptr.Write(tunablesPtr + (index * 8), value);
        }

        for (IniValuePair valuePair : config["int"])
        {
            auto index = std::stoi(valuePair.first);
            auto value = std::stoi(valuePair.second);
            Log("Writing tunable %i at index %i", value, index);
            ptr.Write(tunablesPtr + (index * 8), value);
        }

        PAUSE;
    }

    memDump->Free();

    CloseHandle(pHandle);
}

int main(int argc, char* argv[ ])
{
    std::cout << "Enter Command (edit, dump, sort, enum, ida): ";
    std::string command;
    std::cin >> command;

    if (command == "edit")
    {
        if (argc == 2)
        {
            EditTunables(argv[1]);
        }
        else
        {
            EditTunables("");
        }
    }
    else if (command == "ida")
    {
        if (argc == 2)
        {
            DumpIda(argv[1]);
        }
        else
        {
            DumpIda("");
        }
    }
    else if (command == "dump")
    {
        if (argc == 2)
        {
            DumpTunables(argv[1]);
        }
        else
        {
            DumpTunables("");
        }
    }
    else if (command == "sort")
    {
        std::string inputFileName;
        std::string outputFileName;

        if (argc != 2)
        {
            std::cout << "Enter Input Filename: ";
            std::cin >> inputFileName;
        }
        else
        {
            inputFileName = argv[1];
        }

        std::cout << "Enter Output Filename: ";
        std::cin >> outputFileName;

        SortTunableDump(inputFileName, outputFileName);
    }
    else if (command == "enum")
    {
        std::string inputFileName;
        std::string outputFileName;

        if (argc != 2)
        {
            std::cout << "Enter Input Filename: ";
            std::cin >> inputFileName;
        }
        else
        {
            inputFileName = argv[1];
        }

        std::cout << "Enter Output Filename: ";
        std::cin >> outputFileName;

        CreateEnumFromDump(inputFileName, outputFileName);
    }


    //DumpTunables();

    //TestAll();

    system("PAUSE");

    return 0;
}

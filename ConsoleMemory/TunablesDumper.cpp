#include "stdafx.h"

#include "TunablesDumper.h"

#include "MemDump.h"
#include "ProcessFinder.h"

#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>

#include <iomanip>

#include <regex>

void DumpTunables(std::string fileName)
{
    if (fileName.length() == 0)
    {
        std::cout << "Enter Output File Name: ";
        std::cin >> fileName;
    }

    PROCESSENTRY32 processEntry = GetProcessFromName(L"gta5.exe");
    BrickAssert(processEntry.th32ProcessID != NULL);

    MODULEENTRY32 processModule = GetMainModule(processEntry.th32ProcessID);
    BrickAssert(processModule.th32ProcessID != NULL);

    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
    BrickAssert(pHandle != nullptr);

    RMem ptr(pHandle);

    MemDump* memDump = new MemDump(ptr);

    memDump->DumpModule(processModule, MEM_ALL_ACCESS);

    memDump->Print();

    AOBScanInfo tunableScan("48 8B 8C C2 ? ? ? ? 48 85 C9 74 19");

    uintptr_t tunablesResult = memDump->AOBScan(tunableScan);

    memDump->Free();

    uintptr_t tunablesPtr = ptr.Read<uintptr_t>(uintptr_t(processModule.modBaseAddr) + ptr.Read<int>(tunablesResult + 0x4) + 0x8);

    std::map<int, std::string> commentMap = GetKnownTunables();

    const int dumpSize = 19000;

    std::ofstream outStream(fileName);

    outStream << "Tunables dumped at " << FormatDate("%d.%m") << " by Brick's Tunables Dumper" << std::endl << std::endl;

    const int offsetSpacing = 8, uintSpacing = 10, floatSpacing = 12, indexSpacing = 5;

    char formatBuffer[128];
    sprintf_s(formatBuffer, "| %-*s | %-*s | %-*s | %-*s | %-s", offsetSpacing, "Offset", uintSpacing, "Uint", floatSpacing, "Float", indexSpacing, "Index", "Comment");

    outStream << formatBuffer << std::endl;

    for (int i = 0; i < dumpSize; ++i)
    {
        uintptr_t offset = i * 8;
        uintptr_t addr = tunablesPtr + offset;

        unsigned int uintValue  = ptr.Read<unsigned int>(addr);
        float        floatValue = ptr.Read<float>(addr);

        std::map<int, std::string>::iterator comment = commentMap.find(i);

        if (comment != commentMap.end())
        {
            sprintf_s(formatBuffer, "| %0*I64X | %*u | %*G | %*i | %s",  offsetSpacing, offset, uintSpacing, uintValue, floatSpacing, floatValue, indexSpacing, i, comment->second.c_str());
        }
        else
        {
            sprintf_s(formatBuffer, "| %0*I64X | %*u | %*G | %*i |",     offsetSpacing, offset, uintSpacing, uintValue, floatSpacing, floatValue, indexSpacing, i);
        }

        outStream << formatBuffer << std::endl;
    }

    outStream.flush();
    outStream.close();
}

std::map<int, std::string> GetKnownTunables()
{
    const static std::regex commentRegex("^(\\d+) = (.+)$"); // index = comment

    std::ifstream inStream("KnownTunables.txt");
    std::stringstream bufferStream;

    bufferStream << inStream.rdbuf();

    inStream.close();

    std::map<int, std::string> commentMap;

    std::string currentLine;

    while (std::getline(bufferStream, currentLine, '\n'))
    {
        std::smatch match;

        if (std::regex_match(currentLine, match, commentRegex))
        {
            int toInt = std::stoi(match.str(1));
            std::string commentStr = match.str(2);

            commentMap[toInt] = commentStr;
        }
    }

    return commentMap;
}

struct tunablePair
{
    std::string name;
    int index;
};

void SortTunableDump(std::string listName, std::string outputName)
{
    std::ifstream inputStream(listName);

    const static std::regex pairRegex("^\\s*([a-zA-Z0-9_?]+)\\s*=\\s*(\\d+)[,]\\s*?$");
    const static std::regex unNamedRegex("^\\s*(0x[a-zA-Z0-9]{0,8})\\s*=\\s*(\\d+)[,]\\s*?$");

    std::ifstream inStream(listName);

    std::stringstream bufferStream;
    bufferStream << inStream.rdbuf();

    inStream.close();

    std::vector<tunablePair> tunablePairList;

    std::string currentLine;

    while (std::getline(bufferStream, currentLine, '\n'))
    {
        std::smatch match;
        std::smatch unMatch;

        if (std::regex_match(currentLine, match, pairRegex) && !std::regex_match(currentLine, unMatch, unNamedRegex))
        {
            std::string name = match.str(1);
            int index = std::stoi(match.str(2));

            tunablePairList.push_back({ name, index });
        }
        else
        {
            std::cout << "Invalid Line: " << currentLine << std::endl;
        }
    }

    std::cout << "Sorting " << tunablePairList.size() << " tunables" << std::endl;

    std::sort(tunablePairList.begin(), tunablePairList.end(), 
    [ ] (tunablePair pair1, tunablePair pair2)

    { 
        return pair1.index < pair2.index;
    });

    std::ofstream outStream(outputName);

    char formatBuffer[128];

    for (const tunablePair & pair : tunablePairList)
    {
        sprintf_s(formatBuffer, "%s = %i", pair.name.c_str(), pair.index);

        outStream << formatBuffer << std::endl;
    }
}

void CreateEnumFromDump(std::string listName, std::string outputName)
{
    std::ifstream inputStream(listName);

    const static std::regex pairRegex("^\\s*([a-zA-Z0-9_?]+)\\s*=\\s*(\\d+)[,]?\\s*$");
    const static std::regex hashKeyRegex("^\\s*GAMEPLAY::GET_HASH_KEY\\(.+\\)\\s*=\\s*\\d+[,]?\\s*$");
    const static std::regex unNamedRegex("0x[0-9a-fA-F]{0,8}");

    std::ifstream inStream(listName);

    std::stringstream bufferStream;
    bufferStream << inStream.rdbuf();

    inStream.close();

    std::vector<tunablePair> tunablePairList;

    std::string currentLine;

    while (std::getline(bufferStream, currentLine, '\n'))
    {
        std::smatch match;
        std::smatch unMatch;

        if (std::regex_match(currentLine, match, pairRegex) && !std::regex_match(currentLine, unMatch, hashKeyRegex))
        {
            std::string name = match.str(1);
            int index = std::stoi(match.str(2));

            tunablePairList.push_back({ name, index });
        }
        else
        {
            std::cout << "Invalid Line: " << currentLine << std::endl;
        }
    }

    std::cout << "Generating Enum for " << tunablePairList.size() << " tunables" << std::endl;

    int maxIndex = 0;

    for (tunablePair pair : tunablePairList)
    {
        maxIndex = max(maxIndex, pair.index);
    }

    std::vector<std::string> finalList;

    for (int i = 0; i <= maxIndex; ++i)
    {
        std::string currName = "";

        for (tunablePair pair : tunablePairList)
        {
            if ((pair.index == i))
            {
                std::smatch match;

                if (!std::regex_match(pair.name, match, unNamedRegex))
                {
                    currName = pair.name;
                    break;
                }
            }
        }

        if (currName == "")
        {
            for (tunablePair pair : tunablePairList)
            {
                std::smatch match;

                if ((pair.index == i))
                {
                    currName = std::string("Hash_") + pair.name;
                    break;
                }
            }
        }

        if (currName == "")
        {
            currName = std::string("Unk_") + std::to_string(i);
        }

        finalList.push_back(currName);
    }    

    std::ofstream outStream(outputName);

    outStream << "enum TunablesEnum" << std::endl;
    outStream << "{" << std::endl;

    for (auto i = finalList.begin(), end = finalList.end(); i < end; ++i)
    {
        for (auto j = i->begin(), jEnd = i->end(); j < jEnd; ++j)
        {
            if  (
                    !(
                        ((*j >= 'a') && (*j <= 'z')) ||
                        ((*j >= 'A') && (*j <= 'Z')) ||
                        ((*j >= '0') && (*j <= '9')) ||
                        ((*j == '_'))
                     )
                )
            {
                *j = '_';
            }
        }

        outStream << "    " << *i << "," << std::endl;
    }

    outStream << "};";

    outStream.flush();

    outStream.close();
}

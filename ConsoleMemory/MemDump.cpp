#include "stdafx.h"

#include "MemDump.h"

MemDump::MemDump(RPtr rPtr) : rPtr(rPtr)
{
}

MemDump::~MemDump()
{
    Free();
}

void MemDump::Scan(DWORD protectionFlags)
{
    Free();

    uintptr_t addr = NULL;

    if (rPtr.handle)
    {
        Log("[MemDump][Scan] Dumping Process ID %u", GetProcessId(rPtr.handle));

        while (true)
        {
            MEMORY_BASIC_INFORMATION memInfo = rPtr.QueryAddress(addr);

            if (memInfo.RegionSize > 0)
            {
                if ((memInfo.RegionSize > 0) && (memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
                {
                    MemBlockPtr mB = MemBlockPtr(new MemBlock(rPtr, uintptr_t(memInfo.BaseAddress), memInfo.RegionSize));
                    mB->Update();
                    memBlockList.push_back(mB);
                }

                uintptr_t tempaddr = uintptr_t(memInfo.BaseAddress) + memInfo.RegionSize;

                if (tempaddr == addr)
                {
                    break;
                }

                addr = tempaddr;
            }
            else
            {
                break;
            }
        }
    }
}

void MemDump::ScanRange(uintptr_t baseAddress, size_t regionSize, DWORD protectionFlags)
{
    uintptr_t maxAddress = baseAddress + regionSize;

    Free();

    uintptr_t addr = baseAddress;

    if (rPtr.handle)
    {
        Log("[MemDump][Scan] Dumping Process ID %u", GetProcessId(rPtr.handle));

        while (true)
        {
            MEMORY_BASIC_INFORMATION memInfo = rPtr.QueryAddress(addr);

            if (memInfo.RegionSize > 0)
            {
                uintptr_t address = uintptr_t(memInfo.BaseAddress);

                size_t size = memInfo.RegionSize;
                
                uintptr_t regionEnd = address + size;                

                if ((size > 0) && (memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
                {
                    MemBlockPtr mB = MemBlockPtr(new MemBlock(rPtr, address, size));
                    mB->Update();
                    memBlockList.push_back(mB);
                }

                uintptr_t tempaddr = regionEnd;

                if (tempaddr == addr)
                {
                    break;
                }

                if (tempaddr > maxAddress)
                {
                    break;
                }

                addr = tempaddr;
            }
            else
            {
                break;
            }
        }
    }
}

void MemDump::Update()
{
    for (MemBlockPtr block : memBlockList)
    {
        block->Update();
    }
}

void MemDump::Free()
{
    size_t size = memBlockList.size();

    if (size > 0)
    {
        Log("[MemDump][Free] Deleting %I64u blocks", size);

        memBlockList.clear(); // Shared Ptr auto calls MemBlock destructor.
    }
}

void MemDump::Print()
{
    MemDumpInfo info = { };

    for (MemBlockPtr block : memBlockList)
    {
        LogDebug("[MemDump][Print] Block 0x%I64X (0x%I64X)", block->GetRemoteAddress(), block->GetSize());
        info.totalBlockSize += block->GetSize();
        info.blockCount++;
    }

    Log("[MemDump][Print] %I64u Blocks Total. Size: 0x%I64X", info.blockCount, info.totalBlockSize);
}

uintptr_t MemDump::AOBScan(AOBScanInfo pattern)
{
    size_t patternLength = pattern.patternList.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.ToString().c_str());

    for (MemBlockPtr block : memBlockList)
    {        
        std::vector<byte> dumpVector = block->GetByteDump();
        size_t dumpSize = dumpVector.size();

        if (dumpSize > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block->GetRemoteAddress(), block->GetSize());

            for (uintptr_t i = 0; i < dumpSize - patternLength; i += pattern.alignment)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternList[j];
                    if (!pByte.ignore && (dumpVector[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    return block->GetRemoteAddress() + i;
                }
            }
        }
    }

    Log("[MemDump][AOBScan] Failed to find %s", pattern.ToString().c_str());

    return NULL;
}

std::vector<uintptr_t> MemDump::AOBScanArray(AOBScanInfo pattern)
{
    std::vector<uintptr_t> results;

    size_t patternLength = pattern.patternList.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.ToString().c_str());

    for (MemBlockPtr block : memBlockList)
    {

        std::vector<byte> dumpVector = block->GetByteDump();
        size_t dumpSize = dumpVector.size();

        if (dumpSize > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block->GetRemoteAddress(), block->GetSize());

            for (uintptr_t i = 0; i < dumpSize - patternLength; i += pattern.alignment)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternList[j];
                    if (!pByte.ignore && (dumpVector[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    results.push_back(block->GetRemoteAddress() + i);
                }
            }
        }
    }

    return results;
}

RPtr MemDump::GetRPtr()
{
    return rPtr;
}

MemDumpInfo MemDump::GetInfo()
{
    size_t size = 0;
    size_t count = 0;

    for (MemBlockPtr block : memBlockList)
    {
        size += block->GetSize();
        count++;
    }

    return { size, count };
}


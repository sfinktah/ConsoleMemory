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

    MEMORY_BASIC_INFORMATION memInfo;
    uintptr_t addr = NULL;

    if (rPtr.handle)
    {
        Log("[MemDump][Scan] Dumping Process ID %u", GetProcessId(rPtr.handle));

        while (true)
        {
            size_t bufferReturn = VirtualQueryEx(rPtr.handle, LPVOID(addr), &memInfo, sizeof(memInfo));

            if (bufferReturn > 0)
            {
                if ((memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
                {
                    MemBlockPtr mB = MemBlockPtr(new MemBlock(rPtr, memInfo));
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
        LogDebug("[MemDump][Print] Block 0x%I64X (0x%I64X)", block->remoteAddress, block->maxSize);
        info.totalBlockSize += block->maxSize;
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
        if (block->maxSize > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block->remoteAddress, block->maxSize);

            for (uintptr_t i = 0; i < block->maxSize - patternLength; ++i)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternList[j];
                    if (!pByte.ignore && (block->dumpArray[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    return block->remoteAddress + i;
                }
            }
        }
    }

    Log("[MemDump][AOBScan] Failed to find %s", pattern.ToString().c_str());

    return NULL;
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
        size += block->maxSize;
        count++;
    }

    return { size, count };
}


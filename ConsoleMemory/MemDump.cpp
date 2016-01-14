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
                    std::shared_ptr<MemBlock> mB = std::shared_ptr<MemBlock>(new MemBlock(rPtr, memInfo));
                    mB->Update();
                    MemBlockList.push_back(mB);
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
    for (std::shared_ptr<MemBlock> block : MemBlockList)
    {
        block->Update();
    }
}

void MemDump::Free()
{
    Log("[MemDump][Free] Deleting %I64u blocks", MemBlockList.size());

    MemBlockList.clear(); // Shared Ptr auto calls MemBlock destructor.
}

void MemDump::Print()
{
    MemDumpInfo info = { };

    for (std::shared_ptr<MemBlock> block : MemBlockList)
    {
        LogDebug("[MemDump][Print] Block 0x%I64X (0x%I64X)", block->remoteAddress, block->maxSize);
        info.totalBlockSize += block->maxSize;
        info.blockCount++;
    }

    Log("[MemDump][Print] %I64u Blocks Total. Size: 0x%I64X", info.blockCount, info.totalBlockSize);
}

uintptr_t MemDump::AOBScan(AOBScanInfo pattern)
{
    size_t patternLength = pattern.patternArray.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.ToString().c_str());

    for (std::shared_ptr<MemBlock> block : MemBlockList)
    {
        if (block->maxSize > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block->remoteAddress, block->maxSize);

            for (uintptr_t i = 0; i < block->maxSize - patternLength; ++i)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternArray[j];
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

    for (std::shared_ptr<MemBlock> block : MemBlockList)
    {
        size += block->maxSize;
        count++;
    }

    return { size, count };
}


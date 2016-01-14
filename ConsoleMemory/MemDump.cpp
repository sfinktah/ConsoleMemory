#include "stdafx.h"
#include "MemDump.h"

MemDump::MemDump(RPtr rPtr) : rPtr(rPtr)
{
}

//MemDump::~MemDump()
//{
//    Free();
//}

void MemDump::Scan(DWORD protectionFlags)
{
    MemBlockList.clear();

    MEMORY_BASIC_INFORMATION memInfo;
    uintptr_t addr = NULL;

    if (rPtr.handle)
    {
        while (true)
        {
            size_t bufferReturn = VirtualQueryEx(rPtr.handle, LPVOID(addr), &memInfo, sizeof(memInfo));

            if (bufferReturn > 0)
            {
                if ((memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
                {
                    MemBlock mB = MemBlock(rPtr, memInfo);
                    mB.Update();
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

void MemDump::DeepScan()
{
    Scan(MEM_ALL_ACCESS);
}

void MemDump::Update()
{
    for (MemBlock & block : MemBlockList)
    {
        block.Update();
    }
}

//void MemDump::FreeBlocks()
//{
//    size_t blockCount = 0;
//    size_t totalSize = 0;
//
//    for (MemBlock* block : MemBlockList)
//    {
//        blockCount++;
//        totalSize += block->size;
//        LogDebug("[MemDump][Free] Deleted 0x%I64X bytes", block->size);
//        //block->Free();
//        delete block;
//    }
//
//    MemBlockList.clear();
//
//    if ((blockCount > 0) || (totalSize > 0))
//    {
//        Log("[MemDump][Free] Deleted %I64u blocks. Total Size: 0x%I64X bytes", blockCount, totalSize);
//    }
//}

void MemDump::Free()
{
    MemBlockList.clear();
}

void MemDump::Print()
{
    MemDumpInfo info = {  };

    for (MemBlock & block : MemBlockList)
    {
        LogDebug("[MemDump][Print] Block 0x%I64X (0x%I64X)", block.address, block.size);
        info.totalBlockSize += block.size;
        info.blockCount++;
    }

    Log("[MemDump][Print] %I64u Blocks Total. Size: 0x%I64X", info.blockCount, info.totalBlockSize);
}

//uintptr_t MemDump::ToLocalAddress(uintptr_t address)
//{
//    for (MemBlock block : MemBlockList)
//    {
//        if ((block.address < address) && ((block.address + block.size) > address))
//        {
//            return uintptr_t(block->buffer + (addr - remoteAddr));
//        }
//    }
//
//    return NULL;
//}

uintptr_t MemDump::AOBScan(AOBScanInfo pattern)
{
    size_t patternLength = pattern.patternArr.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.tostring().c_str());

    for (MemBlock & block : MemBlockList)
    {
        if (block.size > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block.address, block.size);

            for (uintptr_t i = 0; i < block.size - patternLength; ++i)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternArr[j];
                    if (!pByte.ignore && (block.buffer[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    return block.address + i;
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
    for (MemBlock & block : MemBlockList)
    {
        size += block.size;
        count++;
    }
    return { size, count };
}


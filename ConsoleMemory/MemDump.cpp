#include "stdafx.h"
#include "MemDump.h"

MemDump::MemDump(HANDLE pHandle) : handle(pHandle)
{
}

MemDump::~MemDump()
{
    Free();
}

void MemDump::Scan(DWORD protectionFlags)
{
    FreeBlocks();

    MEMORY_BASIC_INFORMATION memInfo;
    BYTE* addr = nullptr;

    if (handle)
    {
        while (true)
        {
            SIZE_T bufferReturn = VirtualQueryEx(handle, addr, &memInfo, sizeof(memInfo));

            if (bufferReturn > 0)
            {
                if ((memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
                {
                    LogDebug("[MemDump][Scan] Copying 0x%I64X (0x%I64X)", DWORD64(memInfo.BaseAddress), memInfo.RegionSize);

                    MemBlock* mB = new MemBlock(handle, memInfo);
                    mB->Update();
                    MemBlockList.push_back(mB);
                }

                BYTE* tempaddr = LPBYTE(memInfo.BaseAddress) + memInfo.RegionSize;

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
    for each (MemBlock* block in MemBlockList)
    {
        block->Update();
    }
}

void MemDump::FreeBlocks()
{
    for each (MemBlock* block in MemBlockList)
    {
        block->Free();
        delete block;
    }

    MemBlockList.clear();
}

void MemDump::Free()
{
    FreeBlocks();
}

void MemDump::Print()
{
    MemDumpInfo info = {  };

    for each (MemBlock* block in MemBlockList)
    {
        LogDebug("[MemDump][Print] Block 0x%I64X (0x%I64X)", DWORD64(block->address), block->size);
        info.totalBlockSize += block->size;
        info.blockCount++;
    }

    Log("[MemDump][Print] %I64u Blocks Total. Size: 0x%I64X", info.blockCount, info.totalBlockSize);
}

BYTE* MemDump::ToLocalAddress(BYTE* address)
{
    DWORD64 addr = DWORD64(address);

    for each (MemBlock* block in MemBlockList)
    {
        DWORD64 remoteAddr = DWORD64(block->address);
        if ((remoteAddr < addr) && ((remoteAddr + block->size) > addr))
        {
            return block->buffer + (addr - remoteAddr);
        }
    }

    return nullptr;
}

RPtr MemDump::AOBScan(AOBScanInfo pattern)
{
    SIZE_T patternLength = pattern.patternArr.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.tostring().c_str());

    for each (MemBlock* block in MemBlockList)
    {
        if (block->size > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", DWORD64(block->address), block->size);

            for (DWORD64 i = 0; i < block->size - patternLength; ++i)
            {
                bool success = true;

                for (SIZE_T j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternArr[j];
                    if (!pByte.ignore && (block->buffer[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    return RPtr(handle, (block->address + i));
                }
            }
        }
    }

    return RPtr(nullptr, nullptr);
}

HANDLE MemDump::GetHandle()
{
    return handle;
}

MemDumpInfo MemDump::GetInfo()
{
    SIZE_T size = 0;
    SIZE_T count = 0;
    for each (MemBlock* block in MemBlockList)
    {
        size += block->size;
        count++;
    }
    return { size, count };
}


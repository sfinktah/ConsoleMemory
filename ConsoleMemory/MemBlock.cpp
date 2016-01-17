#include "stdafx.h"

#include "MemBlock.h"

MemBlock::MemBlock(RPtr rPtr, MEMORY_BASIC_INFORMATION memInfo) : rPtr(rPtr), maxSize(memInfo.RegionSize), remoteAddress(uintptr_t(memInfo.BaseAddress))
{

}

MemBlock::~MemBlock()
{

}

RPtr MemBlock::GetRPtr()
{
    return rPtr;
}

size_t MemBlock::GetSize()
{
    return dumpArray.size();
}

size_t MemBlock::GetMaxSize()
{
    return maxSize;
}

uintptr_t MemBlock::GetRemoteAddress()
{
    return remoteAddress;
}

std::vector<byte> & MemBlock::GetByteDump()
{
    return dumpArray;
}

void MemBlock::Update()
{
    LogDebug("[MemDump][Scan] Copying 0x%I64X (0x%I64X bytes)", remoteAddress, maxSize);

    dumpArray = rPtr.ReadArray<byte>(remoteAddress, maxSize);
}


#include "stdafx.h"

#include "MemBlock.h"

MemBlock::MemBlock(RPtr rPtr, MEMORY_BASIC_INFORMATION memInfo) : rPtr(rPtr), size(memInfo.RegionSize), address(uintptr_t(memInfo.BaseAddress))
{
}

void MemBlock::Update()
{
    LogDebug("[MemDump][Scan] Copying 0x%I64X (0x%I64X bytes)", address, size);

    buffer = rPtr.ReadArray<byte>(address, size);
}


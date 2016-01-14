#pragma once

#include "RemotePointer.h"

class MemBlock
{
public:


    MemBlock(RPtr rPtr, MEMORY_BASIC_INFORMATION memInfo);

    RPtr GetRPtr();
    size_t GetMaxSize();
    uintptr_t GetRemoteAddress();
    std::vector<byte> GetDumpArray();

    void Update();

public:
    RPtr rPtr;
    size_t maxSize;
    uintptr_t remoteAddress;
    std::vector<byte> dumpArray;
};


#pragma once

#include "RemotePointer.h"

class MemBlock
{
public:
    RPtr rPtr;
    size_t size;
    uintptr_t address;
    std::vector<byte> buffer;

    MemBlock(RPtr rPtr, MEMORY_BASIC_INFORMATION memInfo);

    //~MemBlock();

    //void Free();
    void Update();
};


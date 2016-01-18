#pragma once

#include <vector>
#include <memory>

#include "RemotePointer.h"

class MemBlock
{
public:
    MemBlock(RPtr rPtr, uintptr_t remoteAddress, size_t maxSize);
    ~MemBlock();
    RPtr GetRPtr();
    size_t GetSize();
    size_t GetMaxSize();
    uintptr_t GetRemoteAddress();
    std::vector<byte> & GetByteDump();

    void Update();

private:
    RPtr rPtr;
    const uintptr_t remoteAddress;
    const size_t maxSize; // Max Size of dump (not current size)
    std::vector<byte> dumpArray;
};

typedef std::shared_ptr<MemBlock> MemBlockPtr;


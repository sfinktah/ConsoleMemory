#pragma once

#include <vector>
#include <memory>

#include "RemotePointer.h"

class MemBlock
{
public:
    MemBlock(RPtr rPtr, MEMORY_BASIC_INFORMATION memInfo);
    ~MemBlock();
    RPtr GetRPtr();
    size_t GetMaxSize();
    uintptr_t GetRemoteAddress();
    std::vector<byte> GetByteDump();

    void Update();

public:
    RPtr rPtr;
    size_t maxSize;
    uintptr_t remoteAddress;
    std::vector<byte> dumpArray;
};

typedef std::shared_ptr<MemBlock> MemBlockPtr;
typedef std::vector<MemBlockPtr> MemBlockVector;


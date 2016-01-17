#pragma once

#include <Windows.h>

#include "MemBlock.h"
#include "AOBScanInfo.h"
#include "RemotePointer.h"

#define MEM_ALL_ACCESS (\
PAGE_READONLY          |\
PAGE_READWRITE         |\
PAGE_WRITECOPY         |\
PAGE_EXECUTE           |\
PAGE_EXECUTE_READ      |\
PAGE_EXECUTE_READWRITE |\
PAGE_EXECUTE_WRITECOPY )\

struct MemDumpInfo
{
    size_t totalBlockSize;
    size_t blockCount;
};

typedef std::vector<MemBlockPtr>    MemBlockPtrVector;
typedef MemBlockPtrVector::iterator MemBlocPtrIter;

class MemDump
{
private:
    RPtr rPtr;

public:
    MemBlockPtrVector memBlockList;
    
    explicit MemDump(RPtr rPtr);
    ~MemDump();
    void Scan(DWORD protectionFlags = // Executable memory should be fine for most scans.
        (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)); 
    void Update();
    void Free();
    void Print();

    MemBlocPtrIter begin()
    {
        return memBlockList.begin();
    }

    MemBlocPtrIter end()
    {
        return memBlockList.end();
    }

    uintptr_t AOBScan(AOBScanInfo pattern);

    std::vector<uintptr_t> AOBScanArray(AOBScanInfo pattern);

    RPtr GetRPtr();
    MemDumpInfo GetInfo();
};

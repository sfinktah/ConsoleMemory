#pragma once

#include <Windows.h>
#include <vector>
#include <memory>

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

typedef std::vector<std::shared_ptr<MemBlock>> MemBlockVector;

class MemDump
{
private:
    RPtr rPtr;
public:
    MemBlockVector MemBlockList;

    typedef MemBlockVector::iterator        pMemBlockIter;
    typedef MemBlockVector::const_iterator  pConstMemBlockIter;

    explicit MemDump(RPtr rPtr);
    ~MemDump();
    void Scan(DWORD protectionFlags = (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)); // Executable memory should be fine for most scans.
    void Update();
    void Free();
    void Print();

    pMemBlockIter begin()
    {
        return MemBlockList.begin();
    }

    pMemBlockIter end()
    {
        return MemBlockList.end();
    }

    uintptr_t AOBScan(AOBScanInfo pattern);
    RPtr GetRPtr();
    MemDumpInfo GetInfo();
};


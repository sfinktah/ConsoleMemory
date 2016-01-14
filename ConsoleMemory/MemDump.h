#pragma once
#include <Windows.h>
#include <vector>

#include "MemBlock.h"
#include "AOBScanInfo.h"
#include "RemotePointer.h"

#define MEM_ALL_ACCESS (\
PAGE_NOACCESS          |\
PAGE_READONLY          |\
PAGE_READWRITE         |\
PAGE_WRITECOPY         |\
PAGE_EXECUTE           |\
PAGE_EXECUTE_READ      |\
PAGE_EXECUTE_READWRITE |\
PAGE_EXECUTE_WRITECOPY |\
PAGE_GUARD             |\
PAGE_NOCACHE           |\
PAGE_WRITECOMBINE       )

struct MemDumpInfo
{
    SIZE_T totalBlockSize;
    SIZE_T blockCount;
};

class MemDump
{
private:
    HANDLE handle;
public:

    typedef std::vector<MemBlock*> MemBlockVector;

    MemBlockVector MemBlockList;

    typedef MemBlockVector::iterator        pMemBlockIter;
    typedef MemBlockVector::const_iterator  pConstMemBlockIter;

    explicit MemDump(HANDLE pHandle);
    ~MemDump();

    void Scan(DWORD protectionFlags = (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)); // Executable memory should be fine for most scans.
    void DeepScan();
    void Update();
    void FreeBlocks();
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
    
    uintptr_t ToLocalAddress(BYTE* address);
    uintptr_t AOBScan(AOBScanInfo pattern);
    HANDLE GetHandle();
    MemDumpInfo GetInfo();
};


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

class MemDump
{
private:
    HANDLE handle;
public:
    std::vector<MemBlock*> MemBlockList;

    explicit MemDump(HANDLE pHandle);
    ~MemDump();

    void Scan(DWORD protectionFlags = (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)); // Executable memory should be fine for most scans.
    void DeepScan();
    void Update();
    void FreeBlocks();
    void Free();
    void Dump();
    BYTE* ToLocalAddress(BYTE* address);
    RPtr AOBScan(AOBScanInfo pattern);
    RPtr AOBScan(std::string pattern);
    HANDLE GetHandle();
    SIZE_T GetSize();
};


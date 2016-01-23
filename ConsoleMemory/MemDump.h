#pragma once

#include <Windows.h>
#include <TlHelp32.h>

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
PAGE_EXECUTE_WRITECOPY )

struct MemDumpInfo
{
    size_t totalBlockSize;
    size_t blockCount;
};

class MemDump
{
private:
    static const DWORD defaultProtFlags = (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE);

    RPtr rPtr;
public:
    std::vector<MemBlockPtr> memBlockList;
    
    explicit MemDump(RPtr rPtr);
    ~MemDump();

    void Scan(DWORD protectionFlags = defaultProtFlags);
    void ScanRange(uintptr_t baseAddress, size_t regionSize, DWORD protectionFlags = defaultProtFlags);
    void ScanModule(MODULEENTRY32 moduleInfo, DWORD protectionFlags = defaultProtFlags);

    void Update();
    void Free();
    void Print();

    uintptr_t AOBScan(AOBScanInfo pattern);

    std::vector<uintptr_t> AOBScanArray(AOBScanInfo pattern);

    RPtr GetRPtr();
    MemDumpInfo GetInfo();
};

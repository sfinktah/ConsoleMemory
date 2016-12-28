#pragma once

#include "MemBlock.h"
#include "AOBScanInfo.h"
#include "RemotePointer.h"

#include <Windows.h>
#include <TlHelp32.h>

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
    static const DWORD defaultProtFlags = (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_READWRITE );

    RMem rPtr;
public:
    std::vector<MemBlockPtr> memBlockList;
    
    explicit MemDump(RMem rPtr);
    ~MemDump();

    void Dump(DWORD protectionFlags = defaultProtFlags);
    void DumpRange(uintptr_t baseAddress, size_t regionSize, DWORD protectionFlags = defaultProtFlags);
    void DumpModule(MODULEENTRY32 moduleInfo, DWORD protectionFlags = defaultProtFlags);

    void Update();
    void Free();
    void Print();

    uintptr_t AOBScan(AOBScanInfo pattern);

    std::vector<uintptr_t> AOBScanArray(AOBScanInfo pattern);

    RMem GetRPtr();
    MemDumpInfo GetInfo();
};

#include "stdafx.h"

#include "MemDump.h"

#include "ProcessSearcher.h"

MemDump::MemDump(RMem rPtr) : rPtr(rPtr)
{
}

MemDump::~MemDump()
{
    Free();
}

void MemDump::Dump(DWORD protectionFlags)
{
    Free();

    uintptr_t addr = NULL;

    if (rPtr.pHandle)
    {
        Log("[MemDump][Dump] Dumping Process ID %u", GetProcessId(rPtr.pHandle));

        while (true)
        {
            MEMORY_BASIC_INFORMATION memInfo = QueryRemoteAddress(rPtr.pHandle, addr);

            uintptr_t address = uintptr_t(memInfo.BaseAddress);

			if (!memInfo.RegionSize)
				break;

            size_t size = memInfo.RegionSize;

            if (size > 0)
            {
				// Below are flags about the current region of memory. If you want to specifically scan for only 
				// certain things like if the area is writable, executable, etc. you can use these flags to prevent 
				// reading non-desired protection types. 

				auto isCopyOnWrite = ((memInfo.Protect & PAGE_WRITECOPY) != 0 || (memInfo.Protect & PAGE_EXECUTE_WRITECOPY) != 0);
				auto isExecutable = ((memInfo.Protect & PAGE_EXECUTE) != 0 || (memInfo.Protect & PAGE_EXECUTE_READ) != 0 || (memInfo.Protect & PAGE_EXECUTE_READWRITE) != 0 || (memInfo.Protect & PAGE_EXECUTE_WRITECOPY) != 0);
				auto isWritable = ((memInfo.Protect & PAGE_READWRITE) != 0 || (memInfo.Protect & PAGE_WRITECOPY) != 0 || (memInfo.Protect & PAGE_EXECUTE_READWRITE) != 0 || (memInfo.Protect & PAGE_EXECUTE_WRITECOPY) != 0);


				if (
					// My Assertions
					(memInfo.State == MEM_COMMIT && ((memInfo.Protect & PAGE_GUARD) == 0) && ((memInfo.Protect & PAGE_NOACCESS) == 0)) &&
					// Brick's Assertions
					(memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags)
					)
                {
                    MemBlockPtr mB = MemBlockPtr(new MemBlock(rPtr, address, size));
                    mB->Update();
                    memBlockList.push_back(mB);
                }

                uintptr_t tempaddr = address + size;

                if (tempaddr == addr)
                {
                    break;
                }

                addr = tempaddr;
            }
            else
            {
                break;
            }
        }
    }
}

void MemDump::DumpRange(uintptr_t baseAddress, size_t regionSize, DWORD protectionFlags)
{
    Free();

    uintptr_t addr = baseAddress;

    uintptr_t maxAddress = addr + regionSize;

    if (rPtr.pHandle)
    {
        Log("[MemDump][Dump] Dumping Process ID %u from 0x%I64X to 0x%I64X", GetProcessId(rPtr.pHandle), addr, maxAddress);

        while (addr < maxAddress)
        {
            MEMORY_BASIC_INFORMATION memInfo = QueryRemoteAddress(rPtr.pHandle, addr);

            uintptr_t address = uintptr_t(memInfo.BaseAddress);

            size_t size = memInfo.RegionSize;

            if (size > 0)
            {
                if ((memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
                {

                    MemBlockPtr mB = MemBlockPtr(new MemBlock(rPtr, address, size));
                    mB->Update();
                    memBlockList.push_back(mB);
                }

                uintptr_t tempaddr = address + size;

                if (addr == tempaddr)
                {
                    break;
                }

                addr = tempaddr;
            }
            else
            {
                break;
            }
        }
    }
}

void MemDump::DumpModule(MODULEENTRY32 moduleInfo, DWORD protectionFlags)
{
    BrickAssert(moduleInfo.th32ProcessID == GetProcessId(rPtr.pHandle)); // Make sure module is in current process

    return DumpRange(uintptr_t(moduleInfo.modBaseAddr), moduleInfo.modBaseSize, protectionFlags);
}

void MemDump::Update()
{
    for (MemBlockPtr block : memBlockList)
    {
        block->Update();
    }
}

void MemDump::Free()
{
    size_t size = memBlockList.size();

    if (size > 0)
    {
        Log("[MemDump][Free] Deleting %I64u blocks", size);

        memBlockList.clear(); // Shared Ptr auto-calls MemBlock destructor.
    }
}

void MemDump::Print()
{
    MemDumpInfo info = { };

    for (MemBlockPtr block : memBlockList)
    {
        LogDebug("[MemDump][Print] Block 0x%I64X (0x%I64X)", block->GetRemoteAddress(), block->GetSize());
        info.totalBlockSize += block->GetSize();
        info.blockCount++;
    }

    Log("[MemDump][Print] %I64u Blocks Total. Size: 0x%I64X", info.blockCount, info.totalBlockSize);
}

uintptr_t MemDump::AOBScan(AOBScanInfo pattern)
{
    size_t patternLength = pattern.patternList.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.ToString().c_str());

    for (MemBlockPtr block : memBlockList)
    {        
        std::vector<byte> dumpVector = block->GetByteDump();
        size_t dumpSize = dumpVector.size();

        if (dumpSize > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block->GetRemoteAddress(), block->GetSize());

            for (uintptr_t i = 0; i < dumpSize - patternLength; i += pattern.byteAlignment)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternList[j];
                    if (!pByte.ignore && (dumpVector[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    return block->GetRemoteAddress() + i;
                }
            }
        }
    }

    Log("[MemDump][AOBScan] Failed to find %s", pattern.ToString().c_str());

    return NULL;
}

std::vector<uintptr_t> MemDump::AOBScanArray(AOBScanInfo pattern)
{
    std::vector<uintptr_t> results;

    size_t patternLength = pattern.patternList.size();

    Log("[MemDump][AOBScan] Scanning for %s", pattern.ToString().c_str());

    for (MemBlockPtr block : memBlockList)
    {

        std::vector<byte> dumpVector = block->GetByteDump();
        size_t dumpSize = dumpVector.size();

        if (dumpSize > patternLength)
        {
            LogDebug("[MemDump][AOBScan] Scanning Block. Address: 0x%I64X, Size: 0x%I64X", block->GetRemoteAddress(), block->GetSize());

            for (uintptr_t i = 0; i < dumpSize - patternLength; i += pattern.byteAlignment)
            {
                bool success = true;

                for (size_t j = 0; j < patternLength; ++j)
                {
                    PatternByte pByte = pattern.patternList[j];
                    if (!pByte.ignore && (dumpVector[i + j] != pByte.byte))
                    {
                        success = false;
                        break;
                    }
                }

                if (success)
                {
                    results.push_back(block->GetRemoteAddress() + i);
                }
            }
        }
    }

    return results;
}

RMem MemDump::GetRPtr()
{
    return rPtr;
}

MemDumpInfo MemDump::GetInfo()
{
    size_t size = 0;
    size_t count = 0;

    for (MemBlockPtr block : memBlockList)
    {
        size += block->GetSize();
        count++;
    }

    return { size, count };
}


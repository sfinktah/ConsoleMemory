#include "stdafx.h"
#include "ProcessSearcher.h"

MEMORY_BASIC_INFORMATION QueryRemoteAddress(HANDLE hProcess, uintptr_t address)
{
    MEMORY_BASIC_INFORMATION memInfo;

    size_t written = VirtualQueryEx(hProcess, LPVOID(address), &memInfo, sizeof(memInfo));

    BrickAssert(written == sizeof(memInfo));

    return memInfo;
}

std::vector<MEMORY_BASIC_INFORMATION> GetAllPages(HANDLE hProcess, DWORD protectionFlags)
{
    uintptr_t addr = NULL;

    std::vector<MEMORY_BASIC_INFORMATION> pageList;

    while (true)
    {
        MEMORY_BASIC_INFORMATION memInfo = QueryRemoteAddress(hProcess, addr);

        uintptr_t address = uintptr_t(memInfo.BaseAddress);

        size_t size = memInfo.RegionSize;

        if (size > 0)
        {
            if ((memInfo.State & MEM_COMMIT) && (memInfo.Protect & protectionFlags))
            {
                pageList.push_back(memInfo);
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

    return pageList;
}
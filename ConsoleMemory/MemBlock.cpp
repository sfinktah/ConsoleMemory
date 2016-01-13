#include "stdafx.h"

#include <memoryapi.h>
#include "MemBlock.h"

MemBlock::MemBlock(HANDLE hProc, MEMORY_BASIC_INFORMATION memInfo) : hProc(hProc), size(memInfo.RegionSize), address(LPBYTE(memInfo.BaseAddress)), buffer(new BYTE[size])
{
}

MemBlock::~MemBlock()
{
    Free();
}

void MemBlock::Free()
{
    if (buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }
}

void MemBlock::Update()
{
    static BYTE buff[1024 * 1024]; // Static 1MB buffer

    SIZE_T totalRead = 0;
    SIZE_T bytesToRead, bytesRead;

    while (totalRead < size)
    {
        bytesToRead = min((size - totalRead), sizeof(buff));

        if (!ReadProcessMemory(hProc, address + totalRead, buff, bytesToRead, &bytesRead))
        {
            break;
        }

        memcpy(buffer + totalRead, buff, bytesRead);

        totalRead += bytesRead;
    }

    size = totalRead;
}


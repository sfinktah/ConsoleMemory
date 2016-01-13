#pragma once

class MemBlock
{
public:
    HANDLE hProc;
    SIZE_T size;
    BYTE* address;
    BYTE* buffer;

    MemBlock(HANDLE hProc, MEMORY_BASIC_INFORMATION memInfo);

    ~MemBlock();

    void Free();
    void Update();
};


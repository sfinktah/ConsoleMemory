#include "stdafx.h"
#include "RemotePointer.h"

RPtr::RPtr(HANDLE handle, DWORD64 pointer, std::initializer_list<DWORD64> offsets) : pHandle(handle), pointer(pointer)
{
    Offset(NULL, offsets);
}

RPtr::RPtr(HANDLE handle, void* pointer, std::initializer_list<DWORD64> offsets) : RPtr(handle, DWORD64(pointer), offsets)
{

}

void RPtr::Offset(DWORD64 pointerOffset, std::initializer_list<DWORD64> offsets)
{
    pointer += pointerOffset;

    for each (DWORD64 offset in offsets)
    {
        pointer = ReadRemoteMemory<DWORD64>(pHandle, LPVOID(pointer));
        pointer += offset;
    }
}

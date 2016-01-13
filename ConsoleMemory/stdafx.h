// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>
#include <cassert>

#if defined(_DEBUG)
#define Log(format, ...) printf_s(format, __VA_ARGS__)
#else
#define Log(format, ...) (void)
#endif

template <typename T>
T ReadRemoteMemory(HANDLE handle, LPVOID ptr)
{
    T temp;
    assert(ReadProcessMemory(handle, ptr, &temp, sizeof(temp), nullptr));
    return temp;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, LPVOID ptr, T value)
{
    assert(WriteProcessMemory(handle, ptr, value, sizeof(value), nullptr));
}


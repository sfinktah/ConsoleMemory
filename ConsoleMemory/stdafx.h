// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>
#include <cassert>

#define Log(format, ...) printf_s(format, __VA_ARGS__)

#ifdef _DEBUG
#define LogDebug(format, ...) printf_s(format, __VA_ARGS__)
#else
#define LogDebug(format, ...) (void)0
#endif

template <typename T>
T ReadRemoteMemory(HANDLE handle, LPVOID ptr)
{
    T temp = { };
    SIZE_T read = {  };
    BOOL success = ReadProcessMemory(handle, ptr, &temp, sizeof(temp), &read);
    assert(success && (read == sizeof(temp)));
    return temp;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, LPVOID ptr, T value)
{
    SIZE_T write;
    BOOL success = WriteProcessMemory(handle, ptr, value, sizeof(value), &write);
    assert(success && (write == sizeof(value)));
}


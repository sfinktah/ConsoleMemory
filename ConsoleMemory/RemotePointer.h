#pragma once

#include "initializer_list"

template <typename T>
T ReadRemoteMemory(HANDLE handle, LPVOID ptr)
{
    T temp = {};
    SIZE_T read = {};
    BOOL success = ReadProcessMemory(handle, ptr, &temp, sizeof(temp), &read);
    BrickAssert(success && (read == sizeof(temp)), "Failed to Read Memory at 0x%I64X", DWORD64(ptr));
    return temp;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, LPVOID ptr, T value)
{
    SIZE_T write;
    BOOL success = WriteProcessMemory(handle, ptr, value, sizeof(value), &write);
    BrickAssert(success && (write == sizeof(value)), "Failed to Write Memory at 0x%I64X", DWORD64(ptr));
}

struct RPtr
{
    HANDLE pHandle;
    DWORD64 pointer;
public:
    RPtr(HANDLE handle, DWORD64 pointer, std::initializer_list<DWORD64> offsets = {  });
    RPtr(HANDLE handle, void* pointer, std::initializer_list<DWORD64> offsets = {  });

    void Offset(DWORD64 pointerOffset = 0, std::initializer_list<DWORD64> offsets = {  });

    template <typename T> T Get(DWORD64 offset = 0)
    {
        return ReadRemoteMemory<T>(pHandle, LPVOID(pointer + offset));
    }

    template <typename T> void Set(T value, DWORD64 offset = 0)
    {
        WriteRemoteMemory(pHandle, LPVOID(pointer + offset), value);
    }

    //template <typename T, SIZE_T S> void Set(T(&arr)[S], DWORD64 offset = 0) // std::initializer_list works better
    //{
    //    for (SIZE_T i = 0; i < S; ++i)
    //    {
    //        Set(arr[i], offset * sizeof(T));
    //    }
    //}

    template <typename T> void Set(std::initializer_list<T> arr, DWORD64 offset = 0)
    {
        for (SIZE_T i = 0; i < arr.size(); ++i)
        {
            Set(arr[i], offset * sizeof(T));
        }
    }

    operator DWORD64() const
    {
        return pointer;
    }

    operator bool() const
    {
        return pointer != NULL;
    }

    //template<typename T> operator T*() const // Ruins the whole point of requiring the Get/Set (would cause crashing)
    //{
    //    return reinterpret_cast<T*>(pointer);
    //}
};


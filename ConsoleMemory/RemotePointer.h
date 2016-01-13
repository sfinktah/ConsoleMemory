#pragma once

#include "initializer_list"

class RPtr
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


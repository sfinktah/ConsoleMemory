#pragma once

#include "initializer_list"
#include <vector>

//inline DWORD SetProtection(HANDLE handle, uintptr_t ptr, size_t size, DWORD newProtection = PAGE_EXECUTE_READWRITE)
//{
//    DWORD oldProt;
//
//    BOOL success = VirtualProtectEx(handle, LPVOID(ptr), size, newProtection, &oldProt);
//
//    BrickAssert(success, "Failed to set protection at 0x%I64X", ptr);
//
//    return oldProt;
//}

//template <typename T> DWORD SetProtection(HANDLE handle, uintptr_t ptr, DWORD newProtection = PAGE_EXECUTE_READWRITE)
//{
//    return SetProtection(handle, ptr, sizeof(T), newProtection);
//}

template <typename T>
T ReadRemoteMemory(HANDLE handle, uintptr_t ptr, size_t* read = nullptr)
{
    T temp;

    BOOL success = ReadProcessMemory(handle, LPVOID(ptr), &temp, sizeof(temp), read);

    BrickAssert(success, "Failed to Read Memory at 0x%I64X", ptr);
    return temp;
}

template <typename T>
std::vector<T> ReadRemoteMemoryArray(HANDLE handle, uintptr_t ptr, size_t size)
{
    T buffer[512 * 1024] = {  };
    std::vector<T> vector = std::vector<T>(size);
    size_t totalRead = 0;
     
    while (totalRead < size)
    {
        size_t toRead = min(size - totalRead, sizeof(buffer)), read = 0;

        BOOL success = ReadProcessMemory(handle, LPVOID(ptr + totalRead), buffer, toRead, &read);

        BrickAssert(success, "Failed to Read Memory Array at 0x%I64X", ptr + totalRead);

        std::copy_n(buffer, read, vector.begin() + totalRead);

        totalRead += toRead;
    }

    vector.shrink_to_fit();

    return vector;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, uintptr_t ptr, T value, size_t* write = nullptr)
{
    BOOL success = WriteProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), write);

    BrickAssert(success, "Failed to Write Memory at 0x%I64X", ptr);
}

struct RPtr
{
    HANDLE handle;
public:
    explicit RPtr(HANDLE handle) : handle(handle)
    {
    }

    uintptr_t Offset(uintptr_t ptr, std::initializer_list<uintptr_t> offsets)
    {
        for (uintptr_t offset : offsets)
        {
            ptr = Read<uintptr_t>(ptr);
            ptr += offset;
        }
        return ptr;
    }

    template <typename T> T Read(uintptr_t ptr)
    {
        return ReadRemoteMemory<T>(handle, ptr);
    }

    template <typename T> void Write(uintptr_t ptr, T value)
    {
        WriteRemoteMemory(handle, ptr, value);
    }

    template <typename T> std::vector<T> ReadArray(uintptr_t ptr, size_t size)
    {
        return ReadRemoteMemoryArray<T>(handle, ptr, size);
    }

    template <typename T> void WriteArray(uintptr_t ptr, std::vector<T> arr)
    {
        for (size_t i = 0; i < arr.size(); ++i)
        {
            Write(ptr * (i * sizeof(T)), arr[i]);
        }
    }

    //std::string ReadString(uintptr_t ptr)
    //{
    //    std::vector<char> strArr;
    //    while (true)
    //    {
    //        char read = Read<char>(ptr++);
    //        strArr.push_back(read);
    //        if (read == '\0')
    //        {
    //            break;
    //        }
    //    }
    //    return std::string(strArr.data());
    //}

    //void WriteString(uintptr_t ptr, std::string string)
    //{
    //    for (size_t i = 0; i < string.size(); ++i)
    //    {
    //        Write(ptr + i, string[i]);
    //    }
    //}
};


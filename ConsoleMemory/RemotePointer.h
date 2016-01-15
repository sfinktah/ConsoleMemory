#pragma once

#include "initializer_list"
#include <vector>
#include <array>

#define buffer(T) T buffer [(512 / sizeof(T)) * 1024]

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
    buffer(T);

    std::vector<T> vector(size);
    size_t totalRead = 0;

    while (totalRead < size)
    {
        size_t toRead = min(size - totalRead, sizeof(buffer) / sizeof(T));
        size_t read = 0;

        BOOL success = ReadProcessMemory(handle, LPVOID(ptr + totalRead), buffer, toRead * sizeof(T), &read);

        BrickAssert(success, "Failed to Read Memory Array at 0x%I64X", ptr + totalRead);

        std::copy_n(buffer, read / sizeof(T), vector.begin() + totalRead);

        totalRead += toRead;
    }

    vector.shrink_to_fit();

    return vector;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, uintptr_t ptr, T value, size_t* wrote = nullptr)
{
    BOOL success = WriteProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), wrote);

    BrickAssert(success, "Failed to Write Memory at 0x%I64X", ptr);
}

template <typename T>
void WriteRemoteMemoryArray(HANDLE handle, uintptr_t ptr, std::vector<T> vector)
{
    buffer(T);

    size_t arrSize = vector.size();
    size_t totalWrote = 0;

    while (totalWrote < arrSize)
    {
        size_t toWrite = min(arrSize - totalWrote, sizeof(buffer) / sizeof(T));

        std::copy_n(vector.begin() + totalWrote, toWrite, buffer);

        BOOL success = WriteProcessMemory(handle, LPVOID(ptr + totalWrote), buffer, toWrite * sizeof(T), nullptr);

        BrickAssert(success, "Failed to Read Memory Array at 0x%I64X", ptr + totalWrote);

        totalWrote += toWrite;
    }
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
        WriteRemoteMemoryArray(handle, ptr, arr);
    }
};


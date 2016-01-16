#pragma once

#include "initializer_list"
#include <vector>
#include <array>

// Create a static 512KB buffer (per type)
#define buffer(T)                                   \
constexpr size_t bufferSize = (524288 / sizeof(T)); \
static T buffer [bufferSize];                       \

template <typename T>
T ReadRemoteMemory(HANDLE handle, uintptr_t ptr)
{
    T temp;

    size_t readAmt = 0;

    BOOL success = ReadProcessMemory(handle, LPVOID(ptr), &temp, sizeof(temp), &readAmt);

    BrickAssert((success && (readAmt == sizeof(T))), "Failed to Read Memory at 0x%I64X", ptr);

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
        size_t toRead = min(size - totalRead, bufferSize);
        size_t toReadSize = toRead * sizeof(T);
        size_t readSize = 0;

        BOOL success = ReadProcessMemory(handle, LPVOID(ptr + totalRead), buffer, toReadSize, &readSize);

        size_t read = readSize / sizeof(T);

        BrickAssert((success && (readSize == toReadSize)), "Failed to Read Memory Array at 0x%I64X", ptr + totalRead);

        std::copy_n(buffer, read, vector.begin() + totalRead);

        totalRead += toRead;
    }

    vector.shrink_to_fit();

    return vector;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, uintptr_t ptr, T value)
{
    size_t wroteAmt = 0;

    BOOL success = WriteProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), &wroteAmt);

    BrickAssert((success && (wroteAmt == sizeof(T))), "Failed to Write Memory at 0x%I64X", ptr);
}

template <typename T>
void WriteRemoteMemoryArray(HANDLE handle, uintptr_t ptr, std::vector<T> vector)
{
    buffer(T);

    size_t arrSize = vector.size();
    size_t totalWrote = 0;

    while (totalWrote < arrSize)
    {
        size_t toWrite = min(arrSize - totalWrote, bufferSize);
        size_t toWriteSize = toWrite * sizeof(T);
        size_t writeSize = 0;

        std::copy_n(vector.begin() + totalWrote, toWrite, buffer);

        BOOL success = WriteProcessMemory(handle, LPVOID(ptr + totalWrote), buffer, toWriteSize, &writeSize);

        BrickAssert((success && (writeSize == toWriteSize)), "Failed to Read Memory Array at 0x%I64X", ptr + totalWrote);

        totalWrote += toWrite;
    }
}

struct RPtr
{
public:
    HANDLE handle;

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


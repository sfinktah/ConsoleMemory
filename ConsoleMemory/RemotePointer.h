#pragma once

#include <winnt.h>

#include <initializer_list>
#include <vector>

#define buffer(T)                                   \
constexpr size_t bufferSize = (8192 / sizeof(T));   \
T buffer [bufferSize];                              \

template <typename T>
T ReadRemoteMemory(HANDLE handle, uintptr_t ptr)
{
    T temp;

    size_t readAmt = 0;

    BOOL success = ReadProcessMemory(handle, LPVOID(ptr), &temp, sizeof(temp), &readAmt);

    assert(success && (readAmt == sizeof(T)));

    return temp;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, uintptr_t ptr, T & value)
{
    size_t wroteAmt = 0;

    BOOL success = WriteProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), &wroteAmt);

    assert(success && (wroteAmt == sizeof(T)));
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

        assert(success && (readSize == toReadSize)); // May want to comment out, if you are planning to read huge chunks of memory in debug mode

        std::copy_n(buffer, read, vector.begin() + totalRead);

        totalRead += toRead;
    }

    vector.shrink_to_fit();

    return vector;
}

template <typename T>
void WriteRemoteMemoryArray(HANDLE handle, uintptr_t ptr, std::vector<T> & vector)
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

        assert(success && (writeSize == toWriteSize));

        totalWrote += toWrite;
    }
}

static MEMORY_BASIC_INFORMATION QueryRemoteAddress(HANDLE hProcess, uintptr_t address)
{
    MEMORY_BASIC_INFORMATION memInfo;

    size_t written = VirtualQueryEx(hProcess, LPVOID(address), &memInfo, sizeof(memInfo));

    assert(written == sizeof(memInfo));

    return memInfo;
}

struct RPtr
{
public:
    const HANDLE pHandle; // Better to make the handle const, to avoid confusion.

    RPtr(HANDLE handle) : pHandle(handle)
    {

    }

    ~RPtr()
    {
        // Since we are getting parsed the handle instead of creating it, it's better not to close it.

        //BOOL success = CloseHandle(pHandle);

        //assert(success);        
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

    template <typename T>
    T Read(uintptr_t ptr)
    {
        return ReadRemoteMemory<T>(pHandle, ptr);
    }

    template <typename T>
    void Write(uintptr_t ptr, T value)
    {
        WriteRemoteMemory<T>(pHandle, ptr, value);
    }

    template <typename T>
    T ReadPtr(T* ptr)
    {
        return Read<T>(uintptr_t(ptr));
    }

    template <typename T>
    void WritePtr(T* ptr, T value)
    {
        Write<T>(uintptr_t(ptr), value);
    }

    template <typename T>
    T ReadIndex(T* ptr, size_t index)
    {
        return Read<T>(uintptr_t(ptr) + (sizeof(T) * index));
    }

    template <typename T>
    void WriteIndex(T* ptr, size_t index, T value)
    {
        Write<T>(uintptr_t(ptr) + (sizeof(T) * index), value);
    }

    template <typename T>
    std::vector<T> ReadArray(uintptr_t ptr, size_t size)
    {
        return ReadRemoteMemoryArray<T>(pHandle, ptr, size);
    }

    template <typename T>
    void WriteArray(uintptr_t ptr, std::vector<T> arr)
    {
        WriteRemoteMemoryArray<T>(pHandle, ptr, arr);
    }

    MEMORY_BASIC_INFORMATION QueryAddress(uintptr_t ptr)
    {
        return QueryRemoteAddress(pHandle, ptr);
    }

    operator HANDLE()
    {
        return pHandle;
    }
};



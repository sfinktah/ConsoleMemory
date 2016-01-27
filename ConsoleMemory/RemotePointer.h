#pragma once

#include "BrickAssert.h"

#include <Windows.h>

#include <initializer_list>
#include <vector>

static MEMORY_BASIC_INFORMATION QueryRemoteAddress(HANDLE hProcess, uintptr_t address)
{
    MEMORY_BASIC_INFORMATION memInfo;

    size_t written = VirtualQueryEx(hProcess, LPVOID(address), &memInfo, sizeof(memInfo));

    BrickAssert(written == sizeof(memInfo));

    return memInfo;
}

template <typename T>
T ReadRemoteMemory(HANDLE handle, uintptr_t ptr)
{
    T temp;

    size_t readAmt = 0;

    BOOL success = ReadProcessMemory(handle, LPVOID(ptr), &temp, sizeof(temp), &readAmt);

    BrickAssert(success && (readAmt == sizeof(temp)));

    return temp;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, uintptr_t ptr, T & value)
{
    size_t wroteAmt = 0;

    BOOL success = WriteProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), &wroteAmt);

    BrickAssert(success && (wroteAmt == sizeof(value)));
}

template <typename T>
std::vector<T> ReadRemoteMemoryArray(HANDLE handle, uintptr_t ptr, size_t count)
{
    size_t itemSize     = sizeof(T);
    size_t maxReadCount = 8192 / itemSize;

    std::vector<T> vector(count);

    T* inputPtr  = reinterpret_cast<T*>(ptr);
    T* outputPtr = vector.data();

    size_t totalRead = 0;

    while (totalRead < count)
    {
        size_t itemsToRead = min(count - totalRead, maxReadCount);
        size_t bytesToRead = itemsToRead * itemSize;

        size_t bytesRead = 0;

        BOOL success = ReadProcessMemory(handle, inputPtr + totalRead, outputPtr + totalRead, bytesToRead, &bytesRead);

        BrickAssert(success && (bytesRead == bytesToRead));

        totalRead += itemsToRead;
    }

    return vector;
}

template <typename T>
void WriteRemoteMemoryArray(HANDLE handle, uintptr_t ptr, std::vector<T> & vector)
{
    size_t itemSize = sizeof(T);
    size_t maxWriteCount = 8192 / itemSize;

    size_t count = vector.size();

    T* inputPtr  = vector.data();
    T* outputPtr = reinterpret_cast<T*>(ptr);

    size_t totalWrote = 0;

    while (totalWrote < count)
    {
        size_t itemsToWrite = min(count - totalWrote, maxWriteCount);
        size_t bytesToWrite = itemsToWrite * itemSize;

        size_t bytesWrote = 0;

        BOOL success = WriteProcessMemory(handle, outputPtr + totalWrote, inputPtr + totalWrote, bytesToWrite, &bytesWrote);

        BrickAssert(success && (bytesWrote == bytesToWrite));

        totalWrote += itemsToWrite;
    }
}

class RPtr
{
public:
    const HANDLE pHandle; // Better to make the handle const, to avoid confusion.

    RPtr(HANDLE handle) : pHandle(handle)
    {

    }

    static RPtr LocalPtr()
    {
        return RPtr(GetCurrentProcess());
    }

    ~RPtr()
    {
        // Since we are getting parsed the handle instead of creating it, it's better not to close it.

        //BOOL success = CloseHandle(pHandle);

        //BrickAssert(success);        
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

    std::string ReadString(uintptr_t ptr, size_t maxLength)
    {
        std::vector<char> charVector = ReadArray<char>(ptr, maxLength + 1); // +1 for null char

        charVector.back() = '\0'; // Make sure the array is null terminated

        return std::string(charVector.data()); // Return a string (automatically sized to first null char)
    }

    void WriteString(uintptr_t ptr, std::string string)
    {
        std::vector<char> toWrite(string.begin(), string.end());
        toWrite.push_back('\0');

        WriteArray<char>(ptr, toWrite);
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



#pragma once

#include <Windows.h>

#include <vector>
#include <initializer_list>

template <typename T>
T ReadRemoteMemory(HANDLE handle, uintptr_t ptr)
{
    T value;

    size_t bytesRead = 0;

    BOOL success = ReadProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), &bytesRead);

    BrickAssert(success && (bytesRead == sizeof(value)));

    return value;
}

template <typename T>
void WriteRemoteMemory(HANDLE handle, uintptr_t ptr, T & value)
{
    size_t bytesWrote = 0;

    BOOL success = WriteProcessMemory(handle, LPVOID(ptr), &value, sizeof(value), &bytesWrote);

    BrickAssert(success && (bytesWrote == sizeof(value)));
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
    size_t maxWriteCount = 8192 / itemSize; // Max of 8KB per write

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

class RMem
{
public:
    const HANDLE pHandle; // Better to make the handle const, to avoid confusion.

    RMem(HANDLE handle) : pHandle(handle)
    {

    }

    static RMem LocalPtr()
    {
        return RMem(GetCurrentProcess());
    }

    ~RMem()
    {
        // Since we are getting parsed the handle instead of creating it, it's better not to close it.

        //BOOL success = CloseHandle(pHandle);

        //BrickAssert(success);        
    }

    uintptr_t Offset(uintptr_t ptr, std::initializer_list<uintptr_t> offsets)
    {
        for (uintptr_t offset : offsets)
        {
            ptr = Read<uintptr_t>(ptr) + offset;
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
    T ReadIndex(uintptr_t ptr, size_t index)
    {
        return ReadPtr<T>(reinterpret_cast<T*>(ptr) + index);
    }

    template <typename T>
    void WriteIndex(uintptr_t ptr, size_t index, T value)
    {
        WritePtr<T>(reinterpret_cast<T*>(ptr) + index, value);
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
        std::vector<char> charVector = ReadArray<char>(ptr, maxLength);
        charVector.push_back('\0');

        return std::string(charVector.data());
    }

    void WriteString(uintptr_t ptr, std::string string)
    {
        std::vector<char> charVector(string.begin(), string.end());
        charVector.push_back('\0');

        WriteArray<char>(ptr, charVector);
    }

    operator HANDLE()
    {
        return pHandle;
    }
};



#include "stdafx.h"

#include "UnitTests.h"
#include "IniConfig.h"
#include "RemotePointer.h"

void TestAll()
{
    TestPointerAccess();
    TestArrayAccess();
    TestIniConfig();
    TestIndexAccess();
    TestStringAccess();
    TestOther();
}

void TestPointerAccess()
{
    Log("Testing ReadPtr and WritePtr");

    const int initial = 42;
    const int after = 1337;

    RMem ptr = RMem::LocalPtr();

    int i = initial;

    BrickAssert(i == ptr.ReadPtr(&i));

    ptr.WritePtr(&i, after);

    BrickAssert(i == after);

    Log("ReadPtr and WritePtr test success");
}

void TestArrayAccess()
{
    Log("Testing ReadArray and WriteArray");

    const int arrSize = 10000;

    RMem ptr = RMem::LocalPtr();

    int arr[arrSize];

    for (int i = 0; i < arrSize; ++i)
    {
        arr[i] = i;
    }

    std::vector<int> read = ptr.ReadArray<int>(uintptr_t(&arr), arrSize);

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(read[i] == i);
    }

    std::vector<int> toWrite(arrSize);

    for (int i = 0; i < arrSize; ++i)
    {
        toWrite[i] = (i * 10);
    }

    ptr.WriteArray(uintptr_t(&arr), toWrite);

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(arr[i] == (i * 10));
    }

    Log("ReadArray and WriteArray test success");
}

void TestIniConfig()
{
    IniConfig config = IniConfig::FromString("[int]\n1337 = 420\n\n[float]\n163 = 199999.0");

    for (IniValuePair valuePair : config["float"])
    {
        int index = std::stoi(valuePair.first);
        float value = std::stof(valuePair.second);
        Log("Float %f to index %i", value, index);
    }

    for (IniValuePair valuePair : config["int"])
    {
        int index = std::stoi(valuePair.first);
        int value = std::stoi(valuePair.second);
        Log("Int %i to index %i", value, index);
    }

    config["int"]["42"] = std::to_string(1337);

    Log("As String\n%s", config.ToString().c_str());
}

void TestIndexAccess()
{
    Log("Testing ReadIndex and WriteIndex");

    const int arrSize = 10;

    RMem ptr = RMem::LocalPtr();

    int* arr = new int[arrSize];

    for (int i = 0; i < arrSize; ++i)
    {
        arr[i] = i; // Do the intial set
    }

    for (int i = 0; i < arrSize; ++i)
    {
        int read = ptr.ReadIndex<int>(uintptr_t(arr), i);
        BrickAssert(read == i); // BrickAssert the ReadIndex is what it should be
    }

    for (int i = 0; i < arrSize; ++i)
    {
        ptr.WriteIndex<int>(uintptr_t(arr), i, arrSize - i); // Test WriteIndex
    }

    for (int i = 0; i < arrSize; ++i)
    {
        BrickAssert(arr[i] == (arrSize - i)); // BrickAssert the value from WriteIndex is correct
    }

    delete[ ] arr;

    Log("ReadIndex and WriteIndex test success");
}

void TestStringAccess()
{
    Log("Testing ReadString and WriteString");

    RMem ptr = RMem::LocalPtr();

    std::string beginString = "The quick brown fox jumps over the lazy dog";
    std::string endString = "Pack my box with five dozen liquor jugs";

    std::string currString = beginString;

    std::string readString = ptr.ReadString(uintptr_t(currString.data()), currString.length());

    BrickAssert(currString == beginString);

    ptr.WriteString(uintptr_t(currString.data()), endString);

    currString = std::string(currString.data()); // Rescan string, otherwise == would not work (std::string's length is static).

    BrickAssert(currString == endString);

    Log("ReadString and WriteString test sucesss")
}

void TestOther()
{
    // Empty for now
}

#pragma once

#include <string>
#include <sstream>

class IniValue
{
    std::string stringValue;
public:
    IniValue()
    {
        
    }

    IniValue(std::string value) : stringValue(value)
    {

    }

    template <typename T> T Get()
    {
        T value;
        std::stringstream ss;
        ss << stringValue;
        ss >> value;
        return value;
    }

    template <typename T> void Set(T value)
    {
        std::stringstream ss;
        ss << value;
        ss >> stringValue;
    }

    operator std::string() const
    {
        return stringValue;
    }
};


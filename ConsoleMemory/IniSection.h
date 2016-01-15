#pragma once

#include <map>

#include "IniValue.h"

typedef std::map<std::string, IniValue> IniValueMap;
typedef std::pair<std::string, IniValue> IniValuePair;

typedef IniValueMap::iterator IniValueIterator;

class IniSection
{
public:
    IniSection();
    IniSection(IniValueMap valueMap);

    IniValueIterator begin();
    IniValueIterator end();

    IniValue & operator[](std::string name);

private:
    IniValueMap valueMap;
};


#pragma once

#include <string>
#include <map>

typedef std::map<std::string, std::string> IniValueMap;
typedef std::pair<std::string, std::string> IniValuePair;

typedef IniValueMap::iterator IniValueIterator;

class IniSection
{
public:
    IniSection();
    IniSection(IniValueMap valueMap);

    IniValueIterator begin();
    IniValueIterator end();

    std::string & operator[](std::string name);

private:
    IniValueMap valueMap;
};


#pragma once

#include <string>
#include <map>

#include "IniSection.h"

typedef std::map<std::string, IniSection> IniSectionMap;
typedef std::pair<std::string, IniSection> IniSectionPair;

typedef IniSectionMap::iterator IniSectionIterator;

class IniConfig
{
public:
    static IniConfig FromString(std::string string);
    static IniConfig FromFile(std::string file);

    IniConfig();
    explicit IniConfig(IniSectionMap sectionMap);

    IniSectionIterator begin();
    IniSectionIterator end();

    std::string ToString();
    void SaveToFile(std::string fileName);

    IniSection & operator[] (std::string name);
private:
    IniSectionMap sectionMap;
};



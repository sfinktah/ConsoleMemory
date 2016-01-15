#pragma once

#include <map>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include <fstream>

#include "IniSection.h"

typedef std::map<std::string, IniSection> IniSectionMap;
typedef std::pair<std::string, IniSection> IniSectionPair;
typedef IniSectionMap::iterator IniSectionIterator;

class IniConfig
{
public:
    IniConfig();
    explicit IniConfig(IniSectionMap sectionMap);

    static IniConfig FromString(std::string string);
    static IniConfig FromFile(std::string file);

    IniSectionIterator begin();
    IniSectionIterator end();

    IniSection & operator[] (std::string name);
private:
    IniSectionMap sectionMap;
};

#pragma once

#include <string>
#include <map>

typedef std::map<std::string, std::string> IniValueMap;
typedef std::pair<std::string, std::string> IniValuePair;

typedef std::map<std::string, IniValueMap> IniSectionMap;
typedef std::pair<std::string, IniValueMap> IniSectionPair;

class IniConfig : public IniSectionMap
{
public:
    static IniConfig FromString(std::string string);
    static IniConfig FromFile(std::string file);

    void SaveToFile(std::string fileName);
    std::string ToString();
};



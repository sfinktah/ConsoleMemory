#include "stdafx.h"

#include <regex>
#include <sstream>
#include <fstream>

#include "IniConfig.h"

IniConfig::IniConfig()
{
}

IniConfig::IniConfig(IniSectionMap sectionMap) : sectionMap(sectionMap)
{

}

IniConfig IniConfig::FromString(std::string string)
{
    const static std::regex headerRegex = std::regex("\\[([a-zA-Z0-9 ]+)\\]"); // [ string ]
    const static std::regex valueRegex  = std::regex("([a-zA-Z0-9]+)\\s*=\\s*([a-zA-Z0-9,.]+)"); // string1 = string2

    IniConfig config;

    std::stringstream bufferStream;
    bufferStream << string;

    std::string currentLine;

    std::string currentHeader = "";

    while (getline(bufferStream, currentLine, '\n')) // Split string by '\n' (new line)
    {
        std::smatch match;

        if (regex_match(currentLine, match, headerRegex))
        {            
            currentHeader = match.str(1);
        }
        else if (regex_match(currentLine, match, valueRegex))
        {
            std::string key = match.str(1);
            std::string value = match.str(2);

            config[std::string(currentHeader)][std::string(key)] = std::string(value);
        }
    }

    system("PAUSE");

    return config;
}

IniConfig IniConfig::FromFile(std::string file)
{
    IniConfig config;
    std::stringstream bufferStream;

    std::ifstream inputStream(file);
    bufferStream << inputStream.rdbuf();
    inputStream.close();

    return FromString(bufferStream.str());
}

IniSection & IniConfig::operator[](std::string name)
{
    return sectionMap[name];
}

IniSectionIterator IniConfig::begin()
{
    return sectionMap.begin();
}

IniSectionIterator IniConfig::end()
{
    return sectionMap.end();
}

std::string IniConfig::ToString()
{
    std::stringstream stringStream;

    for (IniSectionPair sectionPair : sectionMap)
    {
        std::string sectionName = sectionPair.first;
        IniSection sectionValues = sectionPair.second;

        stringStream << "[" << sectionName << "]" << std::endl;

        for (IniValuePair valuePair : sectionValues)
        {
            stringStream << valuePair.first << " = " << valuePair.second << std::endl;
        }

        stringStream << std::endl;
    }

    return stringStream.str();
}

void IniConfig::SaveToFile(std::string fileName)
{
    std::ofstream fileStream(fileName);

    std::string toString = ToString();

    fileStream << toString;

    fileStream.close();
}

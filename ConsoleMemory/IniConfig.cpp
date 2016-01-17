#include "stdafx.h"

#include <regex>
#include <sstream>
#include <fstream>

#include "IniConfig.h"

IniConfig IniConfig::FromString(std::string string)
{
    const static std::regex headerRegex = std::regex("\\[([a-zA-Z0-9 ]+)\\]"); // [ str(0) ]
    const static std::regex valueRegex = std::regex("([a-zA-Z0-9]+)\\s*=\\s*([a-zA-Z0-9,.]+)"); // str(0) = str(1)

    IniConfig config;

    std::stringstream bufferStream;
    bufferStream << string;

    std::string currentLine;
    std::string currentHeader;

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

            config[currentHeader][key] = value;
        }
        else
        {
            Log("[IniConfig] Invalid Line %s", currentLine.c_str());
        }
    }

    return config;
}

IniConfig IniConfig::FromFile(std::string file)
{
    IniConfig config;

    std::ifstream inputStream(file);
    std::stringstream bufferStream;

    bufferStream << inputStream.rdbuf();

    inputStream.close();

    return FromString(bufferStream.str());
}

std::string IniConfig::ToString()
{
    std::stringstream stringStream;

    for (IniSectionPair sectionPair : *this)
    {
        std::string sectionName = sectionPair.first;
        IniValueMap sectionValues = sectionPair.second;

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

    fileStream << ToString();

    fileStream.close();
}

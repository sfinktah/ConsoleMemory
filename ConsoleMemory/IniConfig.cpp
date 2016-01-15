#include "stdafx.h"
#include "IniConfig.h"

IniConfig::IniConfig()
{
}

IniConfig IniConfig::FromString(std::string string)
{
    static std::regex headerRegex = std::regex("\\[([a-zA-Z0-9 ]+)\\]");
    static std::regex valueRegex = std::regex("([a-zA-Z0-9]+)\\s*=\\s*([a-zA-Z0-9,.]+)");

    IniConfig config;

    std::stringstream bufferStream;
    bufferStream << string;

    std::string currentLine;

    std::string currentHeader = "";

    while (getline(bufferStream, currentLine, '\n'))
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

            config[IniValue(currentHeader)][IniValue(key)] = IniValue(value);
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
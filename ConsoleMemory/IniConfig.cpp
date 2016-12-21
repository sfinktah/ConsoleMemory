#include "stdafx.h"

#include "IniConfig.h"

#include <regex>
#include <sstream>
#include <fstream>

#define IS_STREAM_VALID(stream) (stream.is_open() && stream.good() && !stream.fail())

IniConfig IniConfig::FromString(std::string string)
{
    const static std::regex headerRegex     ("\\[([a-zA-Z0-9 ]+)\\]"); // [ str(0) ]
    const static std::regex valueRegex      ("([a-zA-Z0-9]+)\\s*=\\s*([a-zA-Z0-9,.]+)"); // str(0) = str(1)
    const static std::regex whitespaceRegex ("\\s*");

    IniConfig config;

    std::stringstream bufferStream(string);

    std::string currentLine;
    std::string currentHeader;

    while (getline(bufferStream, currentLine, '\n')) // Split string by '\n' (new line)
    {
        std::smatch match;

        if (regex_match(currentLine, match, headerRegex)) // If line is a section header
        {
            currentHeader = match.str(1);
        }
        else if (regex_match(currentLine, match, valueRegex)) // If line is a value pair
        {
            std::string key = match.str(1);
            std::string value = match.str(2);

            config[currentHeader][key] = value;
        }
        else if (regex_match(currentLine, match, whitespaceRegex)) // If line is just whitespace
        {
            // Ignore whitespace lines
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
    std::ifstream inputStream(file); // Open the file

    BrickAssert(IS_STREAM_VALID(inputStream)); // Check if the file is open, and there wasnt an error

    std::stringstream bufferStream; // Create a stringstream buffer

    bufferStream << inputStream.rdbuf(); // Write the file into the buffer stream

    inputStream.close(); // Close the file

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

    BrickAssert(IS_STREAM_VALID(fileStream));

    fileStream << ToString();

    fileStream.close();
}

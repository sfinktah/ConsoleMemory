#include "stdafx.h"
#include "IniSection.h"

IniSection::IniSection()
{

}

IniSection::IniSection(IniValueMap valueMap) : valueMap(valueMap)
{

}

IniValueIterator IniSection::begin()
{
    return valueMap.begin();
}

IniValueIterator IniSection::end()
{
    return valueMap.end();
}

std::string & IniSection::operator[](std::string name)
{
    return valueMap[name];
}

#pragma once

#include <map>
#include <string>

void DumpTunables(std::string fileName);

std::map<int, std::string> GetKnownTunables();

void SortTunableDump(std::string listName, std::string outputName);

void CreateEnumFromDump(std::string listName, std::string outputName);

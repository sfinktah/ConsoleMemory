#pragma once

#include <Windows.h>

#include <vector>
#include <initializer_list>

MEMORY_BASIC_INFORMATION QueryRemoteAddress(HANDLE hProcess, uintptr_t address);

std::vector<MEMORY_BASIC_INFORMATION> GetAllPages(HANDLE hProcess, DWORD protectionFlags);

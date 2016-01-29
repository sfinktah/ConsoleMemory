#pragma once

#define NOMINMAX

#include "BrickAssert.h"

#include <algorithm>

#define Log(format, ...) printf_s(format, ##__VA_ARGS__); printf_s("\n");

#if defined NDEBUG
#define LogDebug(format, ...) (void)0;
#else
#define LogDebug(format, ...) Log(format, __VA_ARGS__);
#endif

#define PAUSE system("PAUSE");
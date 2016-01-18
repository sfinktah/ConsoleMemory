// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>

#include <cassert>

#define Log(format, ...) printf_s(format, __VA_ARGS__); printf_s("\n");

#if defined _DEBUG
#define LogDebug(format, ...) Log(format, __VA_ARGS__);
#else
#define LogDebug(format, ...) (void)0;
#endif
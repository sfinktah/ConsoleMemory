// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>
#include <stdio.h>

#define Log(format, ...) printf_s(format, __VA_ARGS__); printf_s("\n");

#ifdef _DEBUG
#define LogDebug(format, ...) Log(format, __VA_ARGS__)
#else
#define LogDebug(format, ...) (void)0;
#endif

void _BrickAssert(BOOL condition, PCHAR expression, PCHAR file, PCHAR functionName, INT line, PCHAR errorMsg, ...);

#define FUNC_INFO __FILE__, __FUNCSIG__, __LINE__

#define BrickAssert(condition, errormsg, ...) \
_BrickAssert(condition, #condition, FUNC_INFO, errormsg, ##__VA_ARGS__)


#pragma once

#include <iostream>

#if defined NDEBUG
#define BrickAssert(expression)                                                                 \
if (!(expression))                                                                              \
{                                                                                               \
    std::cout << "Assertion Failed: " << #expression << std::endl;                              \
    abort();                                                                                    \
}
#else
#define BrickAssert(expression)                                                                 \
if (!(expression))                                                                              \
{                                                                                               \
    std::cout << __FILE__ << " line " << __LINE__ << " " << __FUNCTION__ << std::endl;          \
    std::cout << "Assertion Failed: " << #expression << std::endl;                              \
    abort();                                                                                    \
}
#endif

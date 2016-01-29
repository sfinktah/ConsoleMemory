#pragma once

#if defined NDEBUG
#define BrickAssert(expression)                                                                 \
if (!(expression))                                                                              \
{                                                                                               \
    Log("Assertion Failed: %s", #expression);                                                   \
    abort();                                                                                    \
}
#else
#define BrickAssert(expression)                                                                 \
if (!(expression))                                                                              \
{                                                                                               \
    Log("%s line %i %s", __FILE__, __LINE__, __FUNCTION__);                                     \
    Log("Assertion Failed: %s", #expression);                                                    \
    abort();                                                                                    \
}
#endif

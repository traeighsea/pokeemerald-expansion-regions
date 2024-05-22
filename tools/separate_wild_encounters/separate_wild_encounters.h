#ifndef SEPARATE_WILD_ENCOUNTERS_H
#define SEPARATE_WILD_ENCOUNTERS_H

#include <cstdlib>
#include <cstdio>
#include <cstdlib>

#ifdef _MSC_VER

#define FATAL_ERROR(format, ...)          \
do                                        \
{                                         \
    std::fprintf(stderr, format, __VA_ARGS__); \
    std::exit(1);                              \
} while (0)

#else

#define FATAL_ERROR(format, ...)            \
do                                          \
{                                           \
    std::fprintf(stderr, format, ##__VA_ARGS__); \
    std::exit(1);                                \
} while (0)

#endif // _MSC_VER

#endif // SEPARATE_WILD_ENCOUNTERS_H

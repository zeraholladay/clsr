#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define DEBUG_ENABLED 1
#if DEBUG_ENABLED
#define DEBUG(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
#else
#define DEBUG(...) // nothing
#endif

#endif

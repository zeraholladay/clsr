#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define DEBUG_ENABLED 1
#if DEBUG_ENABLED
#define DEBUG(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
#else
#define DEBUG(...) // nothing
#endif

#ifndef ERRMSG
#define ERRMSG(...) fprintf(stderr, "[ERROR] " __VA_ARGS__)
#endif

#endif

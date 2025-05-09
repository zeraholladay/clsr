#ifndef SAFE_SNPRINTF_H
#define SAFE_SNPRINTF_H

#include <stdarg.h>
#include <stdio.h>

#ifndef SAFE_SNPRINTF_BUF_SIZE
#define SAFE_SNPRINTF_BUF_SIZE 1024
#endif

static int safe_snprintf(char *buf, size_t offset, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(buf + offset, SAFE_SNPRINTF_BUF_SIZE - offset, fmt, args);
  va_end(args);
  return (n < 0 || (size_t)n >= SAFE_SNPRINTF_BUF_SIZE) ? 0 : (offset + n);
}

#endif

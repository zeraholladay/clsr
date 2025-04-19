#ifndef ENVIRON_H
#define ENVIRON_H

#include <stddef.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 16
#endif

#include "object.h"

typedef struct EnvironEntry {
  const char *symbol; // symbol name
  int addr;           // heap address
} EnvironEntry;

typedef struct Environ {
  EnvironEntry entries[MAX_ENTRIES];
  int count;
  struct Environ *parent;
} Environ;

Environ *env_new(Environ *parent);
int env_set(Environ *env, const char *sym, int addr);
int env_lookup(Environ *env, const char *s, size_t s_len);

#endif

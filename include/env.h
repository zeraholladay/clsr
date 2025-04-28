#ifndef ENV_H
#define ENV_H

#include <stddef.h>

#ifndef MAX_ENTRIES
#define MAX_ENTRIES 64
#endif

typedef struct EnvEntry {
  const char *symbol; // symbol name
  void *addr;         // heap address
} EnvEntry;

typedef struct Env {
  EnvEntry entries[MAX_ENTRIES];
  int count;
  struct Env *parent;
} Env;

Env *env_new(Env *parent);
int env_set(Env *env, const char *sym, void *addr);
int env_lookup(Env *env, const char *s, void **val);

#endif

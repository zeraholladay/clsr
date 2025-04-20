#include <stdlib.h>
#include <strings.h>

#include "common.h"
#include "env.h"

EnvEntry *_env_lookup(Env *env, const char *sym, size_t sym_len);

Env *env_new(Env *parent) {
  struct Env *env = NULL;
  if (ALLOC(env))
    die(LOCATION);
  memset(env, 0, sizeof(Env));
  env->parent = parent;
  return env;
}

/* perf really matters here. */
EnvEntry *_env_lookup(Env *env, const char *sym, size_t sym_len) {
  for (Env *frame = env; frame != NULL; frame = frame->parent)
    for (int i = 0; i < frame->count; i++)
      if (0 == strncmp_minlen(sym, frame->entries[i].symbol, sym_len))
        return &(frame->entries[i]);
  return NULL;
}

int env_set(Env *env, const char *sym, int addr) {
  if (env->count >= MAX_ENTRIES) {
    die(LOCATION);
  }
  EnvEntry *entity = _env_lookup(env, sym, addr);
  if (entity)
    entity->addr = addr;
  else {
    env->entries[env->count].symbol = sym;
    env->entries[env->count].addr = addr;
    env->count++;
  }
  return 1;
}

int env_lookup(Env *env, const char *sym, size_t sym_len) {
  EnvEntry *entity = _env_lookup(env, sym, sym_len);
  if (entity)
    return entity->addr;
  return -1;
}

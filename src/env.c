#include <stdlib.h>
#include <strings.h>

#include "common.h"
#include "env.h"

// TODO: perf really matters here.
EnvEntry *_env_lookup(Env *env, const char *sym) {
  for (Env *frame = env; frame != NULL; frame = frame->parent)
    for (int i = 0; i < frame->count; i++)
      if (0 == strcmp(sym, frame->entries[i].symbol))
        return &(frame->entries[i]);
  return NULL;
}

Env *env_new(Env *parent) {
  struct Env *env = NULL;
  if (ALLOC(env))
    die(LOCATION);
  memset(env, 0, sizeof(Env));
  env->parent = parent;
  return env;
}

int env_set(Env *env, const char *sym, void *addr) {
  if (env->count >= MAX_ENTRIES) {
    die("Exceeds max number of env entries\n");
  }
  EnvEntry *entity = _env_lookup(env, sym);
  if (entity)
    entity->addr = addr;
  else {
    env->entries[env->count].symbol = sym;
    env->entries[env->count].addr = addr;
    env->count++;
  }
  return 0; // success
}

int env_lookup(Env *env, const char *s, void **val) {
  EnvEntry *entity = _env_lookup(env, s);
  if (entity) {
    *val = entity->addr;
    return 0; // success
  }
  return -1; // fail
}

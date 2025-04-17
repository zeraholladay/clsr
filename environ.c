#include "environ.h"
#include "log.h"

#include <stdlib.h>
#include <strings.h>

env_entry_t *_lookup_env(env_t *env, const char *sym, size_t sym_len);

env_t *env_new(env_t *parent) {
  struct env_t *env = malloc(sizeof(env_t));
  if (!env)
    return NULL;
  memset(env, 0, sizeof(env_t));
  env->parent = parent;
  return env;
}

env_entry_t *_lookup_env(env_t *env, const char *sym, size_t sym_len) {
  for (env_t *frame = env; frame != NULL; frame = frame->parent) {
    for (int i = 0; i < frame->count; i++) {
      int len = strlen(frame->entries[i].symbol) + 1;
      int min_leng = sym_len < len ? sym_len : len;

      if (0 == strncmp(sym, frame->entries[i].symbol, min_leng))
        return &(frame->entries[i]);
    }
  }
  return NULL;
}

int env_set(env_t *env, const char *sym, int addr) {
  return 0;
}

int lookup_env(env_t *env, const char *sym, size_t sym_len) {
  // for (env_t *frame = env; frame != NULL; frame = frame->parent) {
  //   for (int i = 0; i < frame->count; i++) {
  //     int len = strlen(frame->entries[i].symbol) + 1;
  //     int min_leng = sym_len < len ? sym_len : len;

  //     if (0 == strncmp(sym, frame->entries[i].symbol, min_leng))
  //       return frame->entries[i].addr;  //addr
  //   }
  // }
  env_entry_t *entity = _lookup_env(env, sym, sym_len);
  if (entity) return entity->addr;
  return -1;
}

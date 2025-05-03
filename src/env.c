#include <stdlib.h>

#include "common.h"
#include "env.h"

Env *env_new(Env *parent) {
  struct Env *env = NULL;

  if (ALLOC(env))
    die(LOCATION);

  env->parent = parent;
  return env;
}

int env_lookup(Env *env, const char *sym, void **addr) {
  size_t len = strlen(sym);

  for (Env *cur_env = env; cur_env; cur_env = cur_env->parent) {
    rb_node *node = rb_lookup(cur_env->root, sym, len);

    if (node) {
      *addr = RB_VAL(node);
      return 0;
    }
  }

  return -1; // fail
}

int env_set(Env *env, const char *sym, void *addr) {
  size_t len = strlen(sym);

  for (Env *cur_env = env; cur_env; cur_env = cur_env->parent) {
    rb_node *node = rb_lookup(cur_env->root, sym, len);

    if (node) {
      RB_VAL(node) = addr;
      return 0;
    }
  }

  rb_node *node = rb_alloc();

  if (!node)
    die(LOCATION);

  RB_KEY(node) = sym;
  RB_KEY_LEN(node) = len;
  RB_VAL(node) = addr;

  rb_insert(&env->root, node);

  return 0;
}

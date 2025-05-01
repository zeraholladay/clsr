#ifndef ENV_H
#define ENV_H

#include "rb_tree.h"

typedef struct Env {
  struct Env *parent;
  rb_node *root;
} Env;

Env *env_new(Env *parent);
int env_lookup(Env *env, const char *sym, void **addr);
int env_set(Env *env, const char *sym, void *addr);

#endif

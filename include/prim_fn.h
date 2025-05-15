#ifndef PRIM_FN_H
#define PRIM_FN_H

#include <stddef.h>

#define PRIM_FN(name)    prim_fn_lookup(#name, sizeof(#name) - 1)
#define PRIM_FN_LIT(str) prim_fn_lookup(str, sizeof(str) - 1)

// Forward declarations
struct Node;
struct Context;

typedef struct PrimitiveFn {
  const char *name;
  struct Node *(*fn)(struct Node *, struct Context *);
  int arity;
} PrimitiveFn;

/* prim_fn.gperf */
const struct PrimitiveFn *prim_fn_lookup(register const char *str,
                                         register size_t len);
const char *is_in_gperf_keyword_table(const char *text, int state);

#endif

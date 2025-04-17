#include "sym_intern.h"
// #include "log.h"
// #include "strutil.h"

#include <string.h>

struct _node {
  struct _node *left, *right;
  char str[];
} intern_root = {NULL, NULL, ""};

const char *sym_intern(const char *s, size_t s_len) {
  struct _node *cur = &intern_root;

  while (cur) {
    int cmp = strncmp_minlen(s, cur->str, s_len);
    if (cmp == 0)
      return cur->str;
    cur = (cmp < 0) ? cur->left : cur->right;
  }

  struct _node *n = malloc(sizeof(struct _node) + s_len + 1);
  if (!n)
    return NULL;

  n->left = n->right = NULL;

  char *cpy = strncpy(n->str, s, s_len);
  if (!cpy)
    return NULL;

  return cpy;
}

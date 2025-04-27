#include "common.h"

struct _node {
  struct _node *left, *right;
  char *str;
} intern_root = {.left = NULL, .right = NULL, .str = ""};

const char *str_intern(const char *s, size_t s_len) {
  struct _node **cur = &intern_root.left;

  while (*cur) {
    int cmp = strncmp_minlen(s, (*cur)->str, s_len);
    if (cmp == 0)
      return (*cur)->str;
    cur = (cmp < 0) ? &(*cur)->left : &(*cur)->right;
  }

  struct _node *n = NULL;
  if (ALLOC(n) || ALLOC_N(n->str, s_len + 1))
    die(LOCATION);

  n->left = n->right = NULL;
  strncpy(n->str, s, s_len);
  *cur = n;

  return n->str;
}

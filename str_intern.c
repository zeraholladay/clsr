#include "str_intern.h"

#include <stdlib.h>
#include <string.h>

struct _node {
  struct _node *left, *right;
  char str[];
} _intern_root = {NULL, NULL, ""};

struct _node *intern_root = &_intern_root;

const char *str_intern(const char *s, size_t len) {
  struct _node **cur = &intern_root;

  while (*cur) {
    int cmp = strncmp(s, (*cur)->str, len);
    if (cmp == 0 && (*cur)->str[len] == '\0')
      return (*cur)->str;
    cur = (cmp < 0) ? &(*cur)->left : &(*cur)->right;
  }

  struct _node *node = malloc(sizeof(struct _node) + len + 1);
  if (!node)
    return NULL;

  node->left = node->right = NULL;

  char *cpy = strncpy(node->str, s, len);
  if (!cpy)
    return NULL;

  *cur = node;

  return cpy;
}

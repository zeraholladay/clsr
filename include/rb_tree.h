#ifndef RB_TREE_H
#define RB_TREE_H

#include <stdlib.h>

#define RB_KEY(n) ((n)->key)

typedef struct rb_node {
  struct rb_node *left, *right, *parent;
  int color;
  char *key;
} rb_node;

void rb_insert(rb_node **root, rb_node *n);
rb_node *rb_lookup(rb_node *root, const char *key);
rb_node *rb_alloc(void);
rb_node *rb_remove(rb_node **root, rb_node *n);

#endif

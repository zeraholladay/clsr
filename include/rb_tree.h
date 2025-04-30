#ifndef RB_TREE_H
#define RB_TREE_H

#define RB_KEY(n) ((n)->key)
#define RB_VAL(n) ((n)->val)

typedef struct rb_node {
  struct rb_node *left, *right, *parent;
  int color, key;
  void *val;
} rb_node;

void rb_insert(rb_node **root, rb_node *n);
rb_node *rb_lookup(rb_node *root, int key);
rb_node *rb_nalloc(int key, void *val);
rb_node *rb_remove(rb_node **root, rb_node *n);

#endif

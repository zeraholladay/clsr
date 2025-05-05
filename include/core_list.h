#ifndef COR_LIST_H
#define COR_LIST_H

#include "core_def.h"

static inline int is_empty_list(const Node *node) {
  return is_list(node) && node->as.list->count == 0;
}

Node *list_append(Node *node, Node *item);
Node *list_first(Node *node);
Node *list_cons(Pool *p, Node *first, Node *rest);
Node *list_rest(Pool *p, Node *node);

static inline Node *empty_list(Pool *p) { return list_cons(p, NULL, NULL); }

#endif

#ifndef LIST_H
#define LIST_H

#include "core_def.h"

Node *list_append(Node *node, Node *item);
Node *list_cons(Pool *p, Node *first, Node *rest);

#endif
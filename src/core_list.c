#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_list.h"
#include "oom_handlers.h"

#ifndef LIST_INITIAL_CAPACITY
#define LIST_INITIAL_CAPACITY 4
#endif

extern oom_handler_t node_oom_list_handler;

// thanks python
static List *_nlist_alloc(void) {
  List *list = calloc(1, sizeof(List));

  if (!list) {
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  list->items = calloc(LIST_INITIAL_CAPACITY, sizeof *(list->items));

  if (!list->items) {
    free(list), list = NULL;
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }
  list->capacity = LIST_INITIAL_CAPACITY;
  list->count = 0;

  return list;
}

static int _nlist_resize(List *list, size_t min_capacity) {
  size_t new_capacity = list->capacity;

  while (new_capacity < min_capacity) {
    new_capacity += (new_capacity >> 3) + (new_capacity < 9 ? 3 : 6);
  }

  Node **new_nodes = realloc(list->items, new_capacity * sizeof(void *));

  if (!new_nodes) {
    free(list->items), list->items = NULL;
    node_oom_list_handler(NULL, OOM_LOCATION);
    return -1;
  }

  list->items = new_nodes;
  list->capacity = new_capacity;

  return 0;
}

static int _nlist_append(List *list, Node *item) {
  if ((list->count >= list->capacity) &&
      (_nlist_resize(list, list->count + 1) != 0)) {
    return -1;
  }
  list->items[list->count++] = item;
  return 0;
}

static Node *_list_alloc(Pool *p) {
  Node *node = pool_alloc(p);
  node->kind = KIND_LIST;
  List *node_list = _nlist_alloc();
  if (!node_list) {
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }
  node->as.list = node_list;
  return node;
}

Node *list_append(Node *node, Node *item) {
  int fail = _nlist_append(node->as.list, item);
  if (fail) {
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }
  return node;
}

Node *list_first(Node *node) { return node->as.list->items[0]; }

Node *list_cons(Pool *p, Node *first, Node *rest) {
  Node *new_node = _list_alloc(p);

  if (!new_node) {
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  size_t first_count = (first) ? first->as.list->count : 0;
  size_t rest_count = (rest) ? rest->as.list->count : 0;
  size_t new_count = first_count + rest_count;

  List *new_list = new_node->as.list;

  if (_nlist_resize(new_list, new_count) != 0) {
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  if (first_count) {
    void *dst = &new_list->items[0];
    void *src = first->as.list->items;
    memcpy(dst, src, sizeof(Node *) * first_count);
  }

  if (rest_count) {
    void *dst = &new_list->items[first_count];
    void *src = rest->as.list->items;
    memcpy(dst, src, sizeof(Node *) * rest_count);
  }

  return new_node;
}

Node *list_rest(Pool *p, Node *node) {
  Node *new_node = list_cons(p, NULL, NULL);

  if (!new_node) {
    node_oom_list_handler(NULL, OOM_LOCATION);
    return NULL;
  }

  List *list = node->as.list;

  for (size_t i = 1; i < list->count; ++i) {
    new_node = list_append(new_node, list->items[i]);
  }
  return new_node;
}

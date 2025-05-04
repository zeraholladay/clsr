#include <stdio.h>
#include <stdlib.h>

#include "clsr.h"
#include "oom_handlers.h"

extern oom_handler_t obj_oom_list_handler;

// thanks python
ObjList *obj_list_new(void) {
  ObjList *obj_list = calloc(1, sizeof(ObjList));

  if (!obj_list) {
    obj_oom_list_handler(NULL, LOCATION);
    return NULL;
  }

  obj_list->nodes =
      calloc(OBJ_LIST_INITIAL_CAPACITY, sizeof *(obj_list->nodes));

  if (!obj_list->nodes) {
    free(obj_list), obj_list = NULL;
    obj_oom_list_handler(NULL, LOCATION);
    return NULL;
  }
  obj_list->capacity = OBJ_LIST_INITIAL_CAPACITY;
  obj_list->count = 0;
  return obj_list;
}

static int obj_list_resize(ObjList *obj_list, size_t min_capacity) {
  size_t new_capacity = obj_list->capacity;

  while (new_capacity < min_capacity) {
    new_capacity += (new_capacity >> 3) + (new_capacity < 9 ? 3 : 6);
  }

  Obj **new_nodes = realloc(obj_list->nodes, new_capacity * sizeof(void *));

  if (!new_nodes) {
    free(obj_list->nodes), obj_list->nodes = NULL;
    obj_oom_list_handler(NULL, LOCATION);
    return -1;
  }

  obj_list->nodes = new_nodes;
  obj_list->capacity = new_capacity;

  return 0;
}

int obj_list_append(ObjList *obj_list, Obj *obj) {
  if ((obj_list->count >= obj_list->capacity) &&
      (obj_list_resize(obj_list, obj_list->count + 1) != 0)) {
    return -1;
  }
  obj_list->nodes[obj_list->count++] = obj;
  return 0;
}
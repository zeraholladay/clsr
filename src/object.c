#include "object.h"

#include "common.h"

#ifndef HEAP_SIZE
#define HEAP_SIZE 1024 * 4
#endif

Object heap[HEAP_SIZE];
int heap_top = 0;

Object *object_alloc(ObjectKind kind, int *addr) {
  if (heap_top >= HEAP_SIZE) {
    DEBUG("[HEAP] Heap overflow\n"); // TO DO: fix me
    return 0;
  }
  *addr = heap_top;
  Object *obj = &heap[heap_top++];
  obj->kind = kind;
  return obj;
}

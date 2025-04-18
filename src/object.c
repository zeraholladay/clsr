#include "object.h"

#include "log.h"

#ifndef HEAP_SIZE
#define HEAP_SIZE 1024 * 4
#endif

object_t heap[HEAP_SIZE];
int heap_top = 0;

object_t *alloc_object(enum _object_t_enum type, int *addr) {
  if (heap_top >= HEAP_SIZE) {
    DEBUG("[HEAP] Heap overflow\n"); // TO DO: fix me
    return 0;
  }
  *addr = heap_top;
  object_t *obj = &heap[heap_top++];
  obj->type = type;
  return obj;
}

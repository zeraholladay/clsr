#include <errno.h>

#include "clsr.h"
#include "common.h"

ObjPool *obj_pool_init(unsigned int count) {
  ObjPool *p = NULL;

  if (ALLOC(p) || ALLOC_N(p->pool, count))
    die(LOCATION);

  DEBUG("[OBJ_POOL] n=%u sizeof=%lu bytes=%lu\n", count, sizeof *(p->pool),
        count * sizeof *(p->pool));

  for (unsigned int i = 0; i < count - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }

  p->pool[count - 1].next_free = NULL;
  p->free_list = &p->pool[0];
  p->count = count;

  return p;
}

Obj *obj_pool_alloc(ObjPool *p) {
  if (!p->free_list) {
    errno = ENOMEM;
    die(LOCATION);
  }
  ObjPoolWrapper *wrapper = p->free_list;
  p->free_list = wrapper->next_free;
  return &wrapper->obj;
}

void obj_pool_free(ObjPool *p, Obj *obj) {
  ObjPoolWrapper *wrapper =
      (ObjPoolWrapper *)((char *)obj - offsetof(ObjPoolWrapper, obj));
  wrapper->next_free = p->free_list;
  p->free_list = wrapper;
}

void obj_pool_reset(ObjPool *p) {
  for (unsigned int i = 0; i < p->count - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }
  p->pool[p->count - 1].next_free = NULL;
  p->free_list = &p->pool[0];
}

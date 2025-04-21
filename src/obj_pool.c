#include <errno.h>

#include "common.h"
#include "obj.h"

ObjPool *obj_pool_init(unsigned int n) {
  ObjPool *p = NULL;
  if (ALLOC(p) || ALLOC_N(p->pool, n))
    die(LOCATION);

  for (unsigned int i = 0; i < n - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }
  p->pool[n - 1].next_free = NULL;
  p->free_list = &p->pool[0];

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

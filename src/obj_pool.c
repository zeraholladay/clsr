#include <errno.h>

#include "clsr.h"
#include "common.h"

ObjPool *obj_pool_init(unsigned int count) {
  ObjPool *p = NULL;

  if (ALLOC(p) || ALLOC_N(p->pool, count))
    die(LOCATION);

  for (unsigned int i = 0; i < count - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }

  p->pool[count - 1].next_free = NULL;
  p->free_list = &p->pool[0];
  p->count = count;

  return p;
}

void obj_pool_destroy(ObjPool **p) {
  FREE((*p)->pool);
  FREE(*p);
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

unsigned int obj_pool_reset_from_mark(ObjPool *p, ObjPoolWrapper *mark) {
  if (!mark || !p->free_list)
    return 0;

  ObjPoolWrapper *cur = mark;
  ObjPoolWrapper *stop_at = p->free_list;

  unsigned int num_freed;

  for (num_freed = 0; cur != stop_at; ++num_freed) {
    ObjPoolWrapper *next = cur->next_free;
    obj_pool_free(p, &cur->obj);
    cur = next;
  }

  return num_freed;
}

void obj_pool_reset_all(ObjPool *p) {
  for (unsigned int i = 0; i < p->count - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }
  p->pool[p->count - 1].next_free = NULL;
  p->free_list = &p->pool[0];
}

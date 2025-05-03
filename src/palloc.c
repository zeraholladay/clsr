#include <stddef.h>

#include "palloc.h"

Pool *pool_init(size_t count, size_t size) {
  Pool *p = calloc(1, sizeof *(p));

  if (!p)
    return NULL;

  p->pool = calloc(count, sizeof *(p->pool) + size);

  if (!p->pool) {
    free(p), p = NULL;
    return NULL;
  }

  for (unsigned int i = 0; i < count - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }

  p->pool[count - 1].next_free = NULL;
  p->free_list = &p->pool[0];
  p->count = count;

  return p;
}

void pool_destroy(Pool **p) {
  free((*p)->pool), (*p)->pool = NULL;
  free(*p), *p = NULL;
}

void *pool_alloc(Pool *p) {
  if (!p->free_list)
    return NULL;
  Wrapper *wrapper = p->free_list;
  p->free_list = wrapper->next_free;
  return wrapper->ptr;
}

void pool_free(Pool *p, void *ptr) {
  Wrapper *wrapper = (Wrapper *)((void *)ptr - offsetof(Wrapper, ptr));
  wrapper->next_free = p->free_list;
  p->free_list = wrapper;
}

unsigned int pool_reset_from_mark(Pool *p, Wrapper *mark) {
  if (!mark || !p->free_list)
    return 0;

  Wrapper *cur = mark;
  Wrapper *stop_at = p->free_list;

  unsigned int num_freed;

  for (num_freed = 0; cur != stop_at; ++num_freed) {
    Wrapper *next = cur->next_free;
    pool_free(p, &cur->ptr);
    cur = next;
  }

  return num_freed;
}

void pool_reset_all(Pool *p) {
  for (unsigned int i = 0; i < p->count - 1; ++i) {
    p->pool[i].next_free = &p->pool[i + 1];
  }
  p->pool[p->count - 1].next_free = NULL;
  p->free_list = &p->pool[0];
}

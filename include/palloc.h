#ifndef PALLOC_H
#define PALLOC_H

#include <stdlib.h>

typedef struct Wrapper
{
  struct Wrapper *next_free;
  void *ptr;
} Wrapper;

typedef struct
{
  Wrapper *free_list, *pool;
  size_t count, stride;
} Pool;

Pool *pool_init (size_t count, size_t size);
void pool_destroy (Pool **p);
void *pool_alloc (Pool *p);
void pool_free (Pool *p, void *ptr);
unsigned int pool_reset_from_mark (Pool *p, Wrapper *mark);
void pool_reset_all (Pool *p);

#endif

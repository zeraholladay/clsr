#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Object Object;

typedef Object *(*eq_fn) (Object *, Object *);
typedef Object *(*new_fn) (Object *);
typedef Object *(*to_str_fn) (Object *);

typedef struct Type
{
  const char *name;
  eq_fn eq_fn;
  new_fn new_fn;
  to_str_fn to_str_fn;
} Type;

typedef struct Object
{
  Type *type;
} Object;

#endif

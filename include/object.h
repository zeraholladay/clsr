#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Object Object;

typedef Object *(*eq_fn) (Object *, Object *);
typedef Object *(*init_fn) (Object *, Object *args, Object *kwargs);
typedef Object *(*new_fn) (Object *, Object *args, Object *kwargs);
typedef Object *(*to_sym_fn) (Object *);

typedef struct Cls
{
  const char *name;
  Object *base, *attrs;
  eq_fn eq;
  init_fn init;
  new_fn new;
  to_sym_fn to_sym;
} Cls;

typedef struct Object
{
  Cls *cls;
  Object *attrs;
} Object;

Object *sym (const char *str, size_t len);

#endif

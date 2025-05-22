#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Obj Obj;

typedef Obj *(*eq_fn) (Obj *, Obj *);
typedef Obj *(*init_fn) (Obj *, Obj *args, Obj *kwargs);
typedef Obj *(*new_fn) (Obj *, Obj *args, Obj *kwargs);
typedef Obj *(*to_sym_fn) (Obj *);

typedef struct Cls
{
  const char *name;
  Obj *base, *attrs;  // attrs is a pointer to a dict.
  eq_fn eq;
  init_fn init;
  new_fn new;
  to_sym_fn to_sym;
} Cls;

typedef struct Obj
{
  Cls *cls;
  union
  {
    Obj *attrs;  // ptr to dict
    Dict *dict;
  };
} Obj;

Obj *is (Obj *o1, Obj *o2);
Obj *sym (const char *str, size_t len);

#endif

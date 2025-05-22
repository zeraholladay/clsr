#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Obj Obj;

typedef Obj *(*eq_fn) (Obj *, Obj *);
typedef void (*init_fn) (Obj *, Obj *, Obj *);
typedef Obj *(*new_fn) (Obj *, Obj *args, Obj *kwargs);
typedef Obj *(*to_sym_fn) (Obj *);

typedef struct Cls
{
  const char *name;
  Obj *base;
  Dict *attrs;
  eq_fn eq;
  init_fn init;
  new_fn new;
} Cls;

typedef struct Obj
{
  Cls *cls;
  Dict *attrs;
} Obj;

Obj *eq (Obj *self, Obj *other);
Obj *is (Obj *o1, Obj *o2);
Cls *super (Obj *self);
void init (Obj *self, Obj *args, Obj *kwargs);
Obj *new (Obj *cls, Obj *args, Obj *kwargs);
Obj *obj (void);
Obj *nil ();
Obj *clsr (const char *name, size_t len, Obj *base, Obj *dict);
Obj *dict (DictEntity *entities[], size_t n);
Obj *getattr(Obj *obj, const char *key);
Obj *setattr(Obj *obj, const char *key, Obj *val);

#endif

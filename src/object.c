#include "stdlib.h"

#include "object.h"
#include "safe_str.h"
#include "sym_save.h"

Obj *
eq (Obj *self, Obj *other)
{
  return (self == other) ? obj() : nil();
}

Obj *
is (Obj *o1, Obj *o2)
{
  if (!o1 || o2)
    {
      return nil();
    }
  return (o1->cls == o2->cls) ? obj() : nil();
}

Cls *
super (Obj *self)
{
  if (!self || !self->cls || !self->cls->base || !self->cls->base->cls)
    {
      return (Cls *) nil();
    }
  return self->cls->base->cls;
}

void
init (Obj *self, Obj *args, Obj *kwargs)
{
  (void)self;
  (void)args;
  (void)kwargs;
  self->attrs = dict_alloc(NULL, 0);
}

Obj *
new (Obj *cls, Obj *args, Obj *kwargs)
{
  Obj *self = malloc (sizeof *(self));
  if (!self || !cls)
    {
      return nil();
    }
  self->cls = cls->cls;  // point self's class to class
  self->cls->init (self, args, kwargs);
  return self;
}

Obj *
obj (void)
{
  static Cls cls = {
    .name = "cls::obj",
    .base = NULL,
    .attrs = NULL,
    .eq = eq,
    .init = init,
    .new = new,
  };
  cls.base = &cls;
  return new(&cls, NULL, NULL);
}

Obj *
nil ()
{
  static Cls cls = {
    .name = "cls::nil",
    .base = NULL,
    .attrs = NULL,
    .eq = eq,
    .init = NULL,
    .new = NULL,
  };
  static Obj nil = {
    .cls = &cls,
    .attrs = NULL,
  };
}

Obj *
clsr (const char *name, size_t len, Obj *base, Obj *dict)
{
  static Cls cls = {
    .name = "cls::nil",
    .base = NULL,
    .attrs = NULL,
    .eq = eq,
    .init = init,
    .new = new,
  };
  cls.base = obj()->cls;

  Cls *new = malloc (sizeof(Cls));
  if (!new)
    {
      return nil();
    }

  if (!memcpy(new, &cls, sizeof(Cls)))
    {
      return nil();
    }

  new->name = name;
  new->base = base;
  new->attrs = dict;

  return new;
}

Obj *
dict (DictEntity *entities[], size_t n)
{
  Obj *new = obj();
  if (!new)
    {
      return nil();
    }

  for (size_t i = 0; i < n; ++i)
    {
      dict_insert (new->attrs, entities[i]->key, entities[i]->val);
    }

  return new;
}

Obj *
getattr(Obj *obj, const char *key)
{
  if (!obj->attrs)
    {
      return nil();
    }

  DictEntity *entity = dict_lookup (obj->attrs, key);
  if (!entity)
    {
      return nil();
    }

  return (Obj *) entity->key;
}

Obj *
setattr(Obj *obj, const char *key, Obj *val)
{
  if (0 > dict_insert (obj->attrs, key, val))
    {
      return nil();
    }

  return val;
}

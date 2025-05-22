#include "stdlib.h"

#include "object.h"
#include "safe_str.h"
#include "sym_save.h"

// declarations
static Obj *eq (Obj *self, Obj *other);
static Obj *new (Obj *cls, Obj *args, Obj *kwargs);
static Obj *new_nil (Obj * /*unused*/, Obj */*unused*/, Obj */*unused*/);
static Obj *init (Obj *self, Obj *dict, Obj * /*unused*/);
static Obj *init_cls (Obj *self, Obj *args, Obj *kwargs);
static Obj *init_sym (Obj *self, Obj *args, Obj *kwargs);
static Obj *to_sym (Obj *self);
static Obj *to_sym_cls (Obj *self);
static Obj *to_sym_nil (Obj *self);

static Obj obj_cls;
static Obj obj_nil;
static Obj obj_obj;

// static cls structs
static Cls cls_dict = {
  .name = "cls::dict",
  .base = &obj_cls,
  .attrs = NULL,
  .eq = eq,
  .init = init_cls,
  .new = new,
  .to_sym = to_sym_cls,
};

static Cls cls_cls = {
  .name = "cls::cls",
  .base = &obj_obj,
  .attrs = NULL,
  .eq = eq,
  .init = init_cls,
  .new = new,
  .to_sym = to_sym_cls,
};

static Cls cls_nil = {
  .name = "cls::nil",
  .base = NULL,
  .attrs = NULL,
  .eq = eq,
  .init = init_cls,
  .new = new_nil,
  .to_sym = to_sym_nil,
};

static Cls cls_obj = {
  .name = "cls::obj",
  .base = NULL,
  .attrs = NULL,
  .eq = eq,
  .init = init,
  .new = new,
  .to_sym = to_sym,
};

static Cls cls_sym = {
  .name = "cls::sym",
  .base = &obj_cls,
  .attrs = NULL,
  .eq = eq,
  .init = init_sym,
  .new = new,
  .to_sym = to_sym,
};

// static object singletons
static Obj obj_nil = {
  .cls = &cls_nil,
  .dict = NULL,
};

static Obj obj_cls = {
  .cls = &cls_cls,
  .dict = NULL,
};

static Obj obj_obj = {
  .cls = &cls_obj,
  .dict = NULL,
};

static Obj obj_sym = {
  .cls = &cls_sym,
  .dict = NULL,
};

// eq
static Obj *
eq (Obj *self, Obj *other)
{
  return (self == other) ? &obj_obj : &obj_nil;
}

// init
static Obj *
init (Obj *self, Obj *attrs, Obj * unused)
{
  (void)unused;
  self->attrs = attrs;
  return self;
}

static Obj *
init_cls (Obj *self, Obj *base, Obj *dict)
{
  self->cls->base = base;
  self->cls->attrs = dict;
  return self;
}

static Obj *
init_sym (Obj *self, Obj *args, Obj *kwargs)
{
  (void)kwargs;
  self->dict = args;
  return self;
}

// new
static Obj *
new (Obj *cls, Obj *args, Obj *kwargs)
{
  Obj *self = malloc (sizeof *(self));
  if (!self || !cls)
    {
      return &obj_nil;
    }

  self->cls = cls->cls;  // point self's class to class

  return cls->cls->init (self, args, kwargs);
}

static Obj *
new_nil (Obj * self, Obj *args, Obj *kwargs)
{
  (void)self;
  (void)args;
  (void)kwargs;
  return &obj_nil;
}

// to_sym
static Obj *
to_sym (Obj *self)
{
  (void)self;
  static const char *repr = "<obj obj>";
  return sym (repr, sizeof (repr) - 1);
}

static Obj *
to_sym_nil (Obj *self)
{
  (void)self;
  static const char *repr = "<obj NIL>";
  return sym (repr, sizeof (repr) - 1);
}

static Obj *
to_sym_cls (Obj *self)
{
  (void)self;
  static const char *repr = "<obj cls>";
  return sym (repr, sizeof (repr) - 1);
}

// c interfaces
Obj *
dict (const DictEntity *entities, size_t n)
{
  entities = dict_shallow_clone_entities(entities, n);
  if (!entities)
    {
      return &obj_nil;
    }

  Dict *dict = dict_alloc (entities, 1);
  if (!dict)
    {
      free(entities);
      return &obj_nil;
    }

  return (&obj_obj)->cls->new (&cls_dict, dict, NULL);
}

Obj *
cls (const char *name, size_t len, Obj *base, Obj *attrs)
{
  // TODO base must be a Cls obj & base implicity includes obj
  Obj *o = (&obj_cls)->cls->new (&obj_cls, base, attrs);
  o->cls->name = sym_save_static (name, len);
  return o;
}

Obj *
is (Obj *o1, Obj *o2)
{
  return (o1->cls == o2->cls) ? &obj_obj : &obj_nil;
}

Obj *
nil ()
{
  return (&obj_nil)->cls->new (NULL, NULL, NULL);
}

Obj *
obj (void)
{
  return (&obj_cls)->cls->new (&obj_cls, NULL, NULL);
}

Obj *
sym (const char *str, size_t len)
{
  const char *sym = sym_save_static (str, len);

  if (!sym)
    {
      return &obj_nil;
    }

  Obj *args = dict ((DictEntity[]) {
    DICT_ENTITY (SYM_SAVE_LITERAL ("sym"), (void *) sym),
  }, 1);

  if (!args)
    {
      return &obj_nil;
    }

  return (&obj_sym)->cls->new (&obj_sym, args, NULL);
}

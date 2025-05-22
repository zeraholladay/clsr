#include "stdlib.h"

#include "object.h"
#include "safe_str.h"
#include "sym_save.h"

// declarations
static Object *eq (Object *self, Object *other);
static Object *new (Object *cls, Object *args, Object *kwargs);
static Object *new_nil (Object *cls_object, Object *args, Object *kwargs);
static Object *init (Object *self, Object *args, Object *kwargs);
static Object *init_cls (Object *self, Object *args, Object *kwargs);
static Object *init_sym (Object *self, Object *args, Object *kwargs);
static Object *to_sym (Object *self);
static Object *to_sym_cls (Object *self);
static Object *to_sym_nil (Object *self);

static Object object_cls;
static Object object_nil;
static Object object_object;

// static cobjects
static Cls cls_cls = {
  .name = "cls::cls",
  .base = &object_object,
  .eq = eq,
  .init = init_cls,
  .new = new,
  .to_sym = to_sym_cls,
};

static Cls cls_nil = {
  .name = "cls::nil",
  .base = NULL,
  .eq = eq,
  .init = init_cls,
  .new = new_nil,
  .to_sym = to_sym_nil,
};

static Cls cls_object = {
  .name = "cls::object",
  .base = NULL,
  .eq = eq,
  .init = init,
  .new = new,
  .to_sym = to_sym,
};

static Cls cls_sym = {
  .name = "cls::sym",
  .base = &object_cls,
  .eq = eq,
  .init = init_sym,
  .new = new,
  .to_sym = to_sym,
};

// static object objects
static Object object_nil = {
  .cls = &cls_nil,
  .attrs = NULL,
};

static Object object_cls = {
  .cls = &cls_cls,
  .attrs = NULL,
};

static Object object_object = {
  .cls = &cls_object,
  .attrs = NULL,
};

static Object object_sym = {
  .cls = &cls_sym,
  .attrs = NULL,
};

// eq
static Object *
eq (Object *self, Object *other)
{
  return (self == other) ? &object_object : &object_nil;
}

// init
static Object *
init (Object *self, Object *args, Object *kwargs)
{
  (void)self;
  (void)args;
  (void)kwargs;
  return self;
}

static Object *
init_cls (Object *self, Object *base, Object *attrs)
{
  self->cls->base = base;
  self->cls->attrs = attrs;
  return self;
}

static Object *
init_sym (Object *self, Object *args, Object *kwargs)
{
  (void)kwargs;
  self->attrs = args;
  return self;
}

// new
static Object *
new (Object *cls_object, Object *args, Object *kwargs)
{
  Object *self = malloc (sizeof *(self));
  if (!self || !cls_object)
    {
      return &object_nil;
    }

  Cls *cls = cls_object->cls;

  self->cls = cls;

  return cls->init (self, args, kwargs);
}

static Object *
new_nil (Object *self, Object *args, Object *kwargs)
{
  (void)self;
  (void)args;
  (void)kwargs;
  return &object_nil;
}

// to_sym
static Object *
to_sym (Object *self)
{
  (void)self;
  static const char *repr = "<object object>";
  return sym (repr, sizeof (repr) - 1);
}

static Object *
to_sym_nil (Object *self)
{
  (void)self;
  static const char *repr = "<object NIL>";
  return sym (repr, sizeof (repr) - 1);
}

static Object *
to_sym_cls (Object *self)
{
  (void)self;
  static const char *repr = "<object cls>";
  return sym (repr, sizeof (repr) - 1);
}

// c interfaces
Object *
cls (const char *name, size_t len, Object *base, Object *attrs)
{
  // TODO base must be a Cls object & base implicity includes object
  Object *o = (&object_cls)->cls->new (&object_cls, base, attrs);
  o->cls->name = sym_save_static (name, len);
  return o;
}

Object *
nil ()
{
  return (&object_nil)->cls->new (NULL, NULL, NULL);
}

Object *
object (void)
{
  return (&object_cls)->cls->new (&object_cls, NULL, NULL);
}

Object *
sym (const char *str, size_t len)
{
  const char *sym = sym_save_static (str, len);

  if (!sym)
    {
      return &object_nil;
    }

  DictEntity *entities = dict_shallow_clone_entities((DictEntity[]) {
    DICT_ENTITY (SYM_SAVE_LITERAL ("sym"), (void *) sym),
  }, 1);
  if (!entities)
    {
      return &object_nil;
    }

  Dict *args = dict_alloc (entities, 1);
  if (!args)
    {
      return &object_nil;
    }

  return (&object_sym)->cls->new (&object_sym, args, NULL);
}

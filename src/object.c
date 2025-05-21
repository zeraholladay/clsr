#include "stdlib.h"

#include "object.h"
#include "safe_str.h"

static Object *new (Object *type_obj);
static Object *init (Object *type_obj);
static Object *to_str ();

static Type type_object = {
  .name = "type.object",
  .eq_fn = eq,
  .new_fn = new,
  .to_str_fn = to_str,
};

static Type type_nil = {
  .name = "type.none",
  .eq_fn = eq,
  .new_fn = new,
  .to_str_fn = to_str,
};

static Type type_type = {
  .name = "type.type",
  .eq_fn = eq,
  .new_fn = new,
  .to_str_fn = to_str,
};

static Object *
eq (Object *self, Object *other)
{
  return (self == other) ? True : False;
}

static Object *
new (Object *type) // class type
{
  Object *obj = malloc (sizeof *(obj));

  if (!obj)
    {
      return NULL;
    }

  obj->type = type;

  return obj;
}

static Object *
to_str (Object *object)
{
  return "";
}

Object *
mro ()
{
}

Object *
object ()
{
  return new(&type_object);
}

Object *
type (const char *type_name, List *bases, Dict *attrs)
{
}

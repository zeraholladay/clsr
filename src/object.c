#include "stdlib.h"

#include "object.h"

static Object *
eq_wrapper (Object *self, List *args, Dict *kwags)
{

}

static Object *
init_wrapper (Object *self, List *args, Dict *kwags)
{
  
}

static Object *
to_str_wrapper (Object *self, List *args, Dict *kwags)
{
  
}

static Atom eq_wrapper_atom = {
  .atomic_type = TYPE_BUILTIN,
  .fn = eq_wrapper,
};

static Type *object_type = NULL;

Dict *
init_types (void)
{
  Dict *type_dict = dict_alloc (NULL, 0);

  if (!type_dict)
    {
      return NULL;
    }

  Dict *object_attrs = dict_alloc (
      (DictEntity[]){
          DICT_ENTITY ("eq", &eq_wrapper_atom),
          DICT_ENTITY ("init_fn", NULL),
          DICT_ENTITY ("to_str", NULL),
      },
      3);

  object_type = type(type_dict, "object", NULL, object_attrs);

  return type_dict;
}

Object *
call (const char *fn_name, Object *self, List *args, Dict *kwargs)
{
  Type *type = self->type;

  DictEntity *attrs = dict_lookup (type->attrs, fn_name);

  if (!attrs) // TODO: superclass
    {
      return NULL;
    }

  Function fn = attrs->val;

  return fn (self, args, kwargs);
}

Object *
new (Type *type, List *args, Dict *kwargs)
{
  Object *new_obj = malloc (sizeof *(new_obj));

  if (!new_obj)
    {
      return NULL;
    }

  new_obj->type = type;

  return call ("init_fn", new_obj, args, kwargs);
}

Type *
type (Dict *type_dict, const char *type_name, List *bases, Dict *attrs)
{
  // types are immutable for now
  DictEntity *entity = dict_lookup(type_dict, type_name);

  if (entity)
    {
      return entity->val;
    }

  // otherwise create new type
  Type *new_type = malloc (sizeof *(new_type));

  if (!new_type)
    {
      return NULL;
    }

  new_type->type_name = type_name;
  new_type->bases = bases;
  new_type->attrs = attrs;

  int res = dict_insert (type_dict, type_name, new_type);

  if (res < 0)
    {
      return NULL;
    }

  return new_type;
}

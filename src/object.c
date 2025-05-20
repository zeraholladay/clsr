#include "stdlib.h"

#include "object.h"

static Object *
init_fn (Type *type, List *args, Dict *kwargs)
{
  Object *new_obj = malloc (sizeof *(new_obj));

  if (!new_obj)
    {
      return NULL;
    }

  new_obj->type = type;

  InitFn init_fn = dict_lookup(type, "init_fn");

  if (!init_fn)
    {
      return NULL;
    }

  return init_fn(type, args, kwargs);
}

Dict *
type_init (void)
{
  return dict_alloc (NULL, 0);
}

Type *
type (Dict *type_dict, const char *type_name, List *bases, Dict *attrs)
{
  Type *new_type = malloc (sizeof *(new_type));

  if (!new_type)
    {
      return NULL;
    }

  new_type->type_name = type_name;
  new_type->bases = bases;
  new_type->attrs = attrs;
  new_type->init_fn = NULL; // TODO

  int res = dict_insert (type_dict, type_name, new_type);

  if (res < 0)
    {
      return NULL;
    }

  return new_type;
}

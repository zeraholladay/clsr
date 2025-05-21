#include "stdlib.h"

#include "object.h"
#include "safe_str.h"

typedef struct ClassEntry
{
  List *bases;
  Dict *attrs;
} ClassEntry;

static DictEntity *
clone_dict_entries(DictEntity *entries[], size_t n)
{
  DictEntity *clones[] = dict_shallow_clone_entities(entries, n);

  if (!clones)
    {
      return NULL;
    }

  for (size_t i = 0; i < n; ++i)
    {
      clones[i]->key = safe_strndup(entries[i]->key, DICT_STR_MAX_LEN);  //TODO: symbol here

      if (!clones[i]->key)
        {
          for (size_t j = 0; j < i - 1; ++j)
            free(clones[j]->key);
            
          return NULL;
        }
    }

  return clones;
}

Dict *
init_class_tab (void)
{
  Dict *class_tab = dict_alloc (NULL, 0);

  if (!class_tab)
    {
      return NULL;
    }

  // the object super class
  Dict * obj_attrs = dict_alloc (
      (DictEntity[]){
          DICT_ENTITY ("eq", NULL),
          DICT_ENTITY ("new", NULL),
          DICT_ENTITY ("to_str", NULL),
      },
      3);

  if (!obj_attrs)
    {
      return NULL;
    }

  return dict_insert(class_tab, "object", obj_attrs);

  // the type class

  return class_tab;
}

Object *
object (Class *class, Dict *attrs)
{
  Object *obj = malloc (sizeof *(obj));

  if (!obj)
    {
      return NULL;
    }

  obj->class = class;
  obj->attrs = attrs;

  return obj;
}

Object *
type (Dict *class_tab, const char *type_name, List *bases, Dict *attrs)
{
  // types are immutable for now
  DictEntity *entity = dict_lookup (class_tab, type_name);

  if (entity)
    {
      return entity->val;
    }

  Type *type = malloc (sizeof *(type));

  if (!type)
    {
      return NULL;
    }

  type->type_name = type_name;
  type->bases = bases;  // [] object is implicit
  type->attrs = attrs;  // add mro

  if (0 > dict_insert (class_tab, type_name, type))
    {
      return NULL;
    }

  Object *obj = object ();

  if (!obj)
    {
      return NULL;
    }

  obj->type = type;

  return obj;
}

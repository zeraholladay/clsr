#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Type Type;

typedef struct Object
{
  Type *type;
  Dict *attrs;
} Object;

typedef Object *(*InitFn) (Type *type, List *args, Dict *kwargs);

struct Type
{
  const char *type_name;
  List *bases;
  Dict *attrs;
  InitFn init_fn;
};

Dict *type_init (void);
Type *type (Dict *type_dict, const char *type_name, List *bases, Dict *attrs);

#endif

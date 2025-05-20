#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Type Type;
typedef struct Object Object;

typedef Object *(*Function) (Object *self, List *args, Dict *kwargs);

typedef enum
{
  TYPE_NONE,      // special constant
  TYPE_SYMBOL,    // identifiers
  TYPE_INTEGER,   // literal
  TYPE_STRING,    // literal
  TYPE_LIST,      // list
  TYPE_DICT,      // dict
  TYPE_BUILTIN,   // builtin fn
  TYPE_LAMBDA     // user-defined fn
} Atomics;

typedef struct object
{
  Atomics atomic_type;
  union
  {
    Function fn;
  };
  
} Atom;

struct Object
{
  Type *type;
  Dict *attrs;
};

struct Type
{
  const char *type_name;
  List *bases;
  Dict *attrs;
};

Dict *init_types (void);
Object *call (const char *fn_name, Object *self, List *args, Dict *kwargs);
Object *new (Type *type, List *args, Dict *kwargs);
Type *type (Dict *type_dict, const char *type_name, List *bases, Dict *attrs);

#endif

#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"
#include "list.h"

typedef struct Object Object;

struct Object
{
  const char *class_name;
  Dict *attrs;
};

#endif

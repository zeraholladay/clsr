#ifndef MACH_H
#define MACH_H

#include <stdio.h>

#include "debug.h"
#include "op.h"
#include "parse.tab.h"

typedef enum {
  OBJ_INT,
  // OBJ_CLOSURE,
  OBJ_SYMBOL,
  // OBJ_NIL
} object_t_enum;

typedef struct object {
  object_t_enum type;
  union {
    int i;
    const char *symbol;
  };
} object_t;

void push(int val);
int pop();
object_t *alloc_object(object_t_enum type, int *addr);
int eval(const struct op *op_ptr);
void run_operator(const struct op *op_ptr);

#endif
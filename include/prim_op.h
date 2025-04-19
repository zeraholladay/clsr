#ifndef PRIM_OP_H
#define PRIM_OP_H

#include "parser.h"

typedef struct _prim_op {
  yytoken_kind_t code;
  int nargs;
} PrimOp;

const prim_op_t *prim_op_lookup(register const char *str, register unsigned int len);
#endif
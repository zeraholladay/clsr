#ifndef PRIM_OP_H
#define PRIM_OP_H

#include "parser.h"

typedef struct PrimOp {
  yytoken_kind_t tok;
  int nargs;
} PrimOp;

const PrimOp *prim_op_lookup(register const char *str, register unsigned int len);
#endif
#ifndef PRIM_OP_H
#define PRIM_OP_H

#include "parser.h"

typedef struct PrimOp {
  yytoken_kind_t tok;
} PrimOp;

const PrimOp *prim_op_lookup(register const char *str,
                             register unsigned int len);
#endif
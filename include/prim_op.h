#ifndef PRIM_OP_H
#define PRIM_OP_H

typedef struct PrimOp {
  int tok;
} PrimOp;

const PrimOp *prim_op_lookup(register const char *str,
                             register unsigned int len);
#endif
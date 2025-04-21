#ifndef PRIM_OP_H
#define PRIM_OP_H

typedef enum {
  PrimOp_Apply,
  PrimOp_Closure,
  PrimOp_Lookup,
  PrimOp_Push,
  PrimOp_Return,
  PrimOp_Set
} PrimOpCode;

typedef struct PrimOp {
  int tok;
  PrimOpCode op_code;
} PrimOp;

const PrimOp *prim_op_lookup(register const char *str,
                             register unsigned int len);
#endif
#ifndef PRIM_OP_H
#define PRIM_OP_H

typedef struct prim_op {
  int code;
  int nargs;
} prim_op;

const prim_op *prim_op_lookup(register const char *str, register unsigned int len);
#endif
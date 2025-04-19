#ifndef MACH_H
#define MACH_H

#include "prim_op.h"

int eval(const PrimOp *op_ptr);
void run_operator(const PrimOp *op_ptr);

#endif
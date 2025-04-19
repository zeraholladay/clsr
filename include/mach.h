#ifndef MACH_H
#define MACH_H

#include "prim_op.h"

int eval(const prim_op_t *op_ptr);
void run_operator(const prim_op_t *op_ptr);

#endif
#ifndef MACH_H
#define MACH_H

#include "operator.h"

int eval(const struct op *op_ptr);
void run_operator(const struct op *op_ptr);

#endif
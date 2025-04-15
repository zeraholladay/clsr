#ifndef MACH_H
#define MACH_H

#include <stdio.h>

#include "debug.h"
#include "op.h"
#include "parse.tab.h"

void push(int val);
int pop();
int eval(const struct op *op_ptr);
void run_operator(const struct op *op_ptr);

#endif
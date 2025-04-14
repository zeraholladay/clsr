#ifndef MACH_H
#define MACH_H

#include <stdio.h>

#include "debug.h"
#include "parse.tab.h"

void push(int val);
int pop();
void enter_frame();
void run_operator(int op_code);

#endif
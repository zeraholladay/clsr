#ifndef REPL_H
#define REPL_H

#include "eval_ctx.h"

void clsr_init (Context *ctx);
void clsr_destroy (Context *ctx);
int clsr_eval_program (Context *ctx);
int clsr_repl (Context *ctx);
int clsr_main (int argc, char **argv);

#endif
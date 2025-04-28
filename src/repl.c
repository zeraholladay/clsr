#include <stdio.h>

#include "clsr.h"
#include "common.h"
#include "parser.h"

#ifndef REPL_MAIN

extern int yyparse(ParseContext *ctx);

int repl(void) {
  ParseContext parser_ctx;
  reset_parse_context(&parser_ctx);
  parser_ctx.obj_pool = obj_pool_init(4096);

  Stack stack = {};
  STACK_INIT(&stack);
  EvalContext eval_ctx = {
      .stack = &stack,
      .env = env_new(NULL),
  };

  for (;;) {
    int parse_status = yyparse(&parser_ctx);

    if (parse_status == 0) {
      DEBUG("[REPL] Eval\n");
      eval(parser_ctx.root_obj, &eval_ctx);
    }
    if (parser_ctx.lexer_state.eof) {
      DEBUG("[REPL] EOF\n");
      return parse_status;
    }
  }
}

#endif

#ifdef REPL_MAIN

extern int repl(void);

int main(void) { repl(); }

#endif
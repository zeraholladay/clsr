#include <stdio.h>

#include "common.h"
#include "eval.h"
#include "obj.h"
#include "parser.h"

extern int yyparse(ParseContext *ctx);

int repl(void) {
  ParseContext ctx;

  reset_parse_context(&ctx);
  ctx.obj_pool = obj_pool_init(4096);

  for (;;) {
    int parse_status = yyparse(&ctx);

    if (parse_status == 0) {
      DEBUG("[REPL] Eval\n");
    }
    if (ctx.lexer_state.eof) {
      DEBUG("[REPL] EOF\n");
      return parse_status;
    }
  }
}

#ifdef REPL_MAIN

int main(void) { repl(); }

#endif
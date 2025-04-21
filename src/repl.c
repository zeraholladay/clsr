#include <stdio.h>

#include "common.h"
#include "obj.h"
#include "parser.h"

void reset_parse_context(ParseContext *ctx);
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

int main(/*int argc, char *argv[]*/) { return repl(); }

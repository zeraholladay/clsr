#include <stdio.h>

#include "common.h"
#include "obj.h"
#include "parser.h"

extern FILE *yyin;
extern int yyparse(ParseContext *ctx);

int repl(void) {
  ParseContext ctx = {
      .obj_pool = NULL,
      .root_obj = NULL,
      .eof_seen = 0,
  };

  ctx.obj_pool = obj_pool_init(4096);

  for (;;) {
    int parse_status = yyparse(&ctx);

    if (parse_status == 1) {
      fprintf(stderr, "Parse error.\n");
    }

    if (ctx.root_obj) {
      DEBUG("[REPL] Eval\n");
    }
    if (ctx.eof_seen) {
      DEBUG("[REPL] EOF\n");
      return 0;
    }
  }
}

int main(/*int argc, char *argv[]*/) { return repl(); }

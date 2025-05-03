#include <stdio.h>
#include <stdlib.h>

#include "clsr.h"
#include "common.h"
#include "parser.h"
#include "readline.h"

#if YYDEBUG
extern int yydebug;
#endif

#ifndef CLSR_MAIN

#ifndef REPL_BUF_SIZ
#define REPL_BUF_SIZ 8192
#endif

extern FILE *yyin;
extern int yyparse(ClsrContext *ctx);
extern void yylex_destroy(void);

void clsr_init(ClsrContext *ctx) {
  ctx->obj_pool = pool_init(OBJ_POOL_COUNT, sizeof(Obj));
  ctx->eval_ctx.env = env_new(NULL);
  STACK_INIT(ctx->eval_ctx.stack);
  reset_parse_context(ctx);
}

void clsr_destroy(ClsrContext *ctx) {
  reset_parse_context(ctx);
  STACK_FREE(ctx->eval_ctx.stack);
  FREE(ctx->eval_ctx.env);
  pool_destroy(&ctx->obj_pool);
}

int clsr_repl(void) {
  Stack stack = {};
  ClsrContext ctx = {};
  ctx.eval_ctx.stack = &stack;

  clsr_init(&ctx);

  rl_init();

  char full_input[REPL_BUF_SIZ];

  for (;;) {
    int len = rl_readline(full_input, sizeof(full_input));

    if (len < 0) {
      break; // TODO: Something
    }

    yyin = fmemopen((void *)full_input, len, "r");

    reset_parse_context(&ctx);
    int parse_status = yyparse(&ctx);

    yylex_destroy();
    fclose(yyin);

    if (parse_status == 0) {
      Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);

      if (eval_status == obj_true) {
        obj_fprintf(stdout, PEEK(ctx.eval_ctx.stack)), printf("\n");
      } else {
        printf("=>error\n"); // TODO
      }

    } else {
      fprintf(stderr, "Parse failed\n");
      continue; // TODO: syntax error
    }
  }
  return 0;
}

#else

extern int clsr_repl(void);

int main(void) {
#if YYDEBUG
  yydebug = YYDEBUG;
#endif
  return clsr_repl();
}

#endif
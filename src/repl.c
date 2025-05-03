#include <stdio.h>
#include <stdlib.h>

#include "clsr.h"
#include "common.h"
#include "parser.h"
#include "readline.h"
#include "sym_save.h"

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
  static int sym_save_bool = 0;

  if (!sym_save_bool && (sym_save_bool = 1))
    sym_save_init();

  CTX_POOL(ctx) = pool_init(OBJ_POOL_COUNT, sizeof(Obj));
  CTX_ENV(ctx) = env_new(NULL);
  STACK_INIT(CTX_STACK(ctx));
  reset_parse_context(ctx);
}

void clsr_destroy(ClsrContext *ctx) {
  reset_parse_context(ctx);
  STACK_FREE(CTX_STACK(ctx));
  FREE(CTX_ENV(ctx));
  pool_destroy(&CTX_POOL(ctx));
}

int clsr_repl(void) {
  Stack stack = {};
  ClsrContext ctx = {};
  CTX_STACK(&ctx) = &stack;

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
      Obj *eval_status = eval(CTX_PARSE_ROOT(&ctx), &ctx);

      if (eval_status == obj_true) {
        obj_fprintf(stdout, CTX_PEEK(&ctx)), printf("\n");
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
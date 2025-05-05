#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "core_def.h"
#include "eval.h"
#include "parser.h"
#include "readline.h"
#include "sym_save.h"

#ifndef OBJ_POOL_CAPACITY
#define OBJ_POOL_CAPACITY 4096
#endif

#if YYDEBUG
extern int yydebug;
#endif

#ifndef CLSR_MAIN

#ifndef REPL_BUF_SIZ
#define REPL_BUF_SIZ 8192
#endif

extern Node *const const_false;
extern Node *const const_true;

extern FILE *yyin;
extern int yyparse(Context *ctx);
extern void yylex_destroy(void);

void clsr_init(Context *ctx) {
  static int sym_save_bool = 0;

  if (!sym_save_bool && (sym_save_bool = 1))
    sym_save_init();

  CTX_POOL(ctx) = pool_init(OBJ_POOL_CAPACITY, sizeof(Node));
  CTX_ENV(ctx) = env_new(NULL);
  // STACK_INIT(CTX_STACK(ctx));
  reset_parse_context(ctx);
}

void clsr_destroy(Context *ctx) {
  reset_parse_context(ctx);
  // STACK_FREE(CTX_STACK(ctx));
  free(CTX_ENV(ctx)), CTX_ENV(ctx) = NULL;
  pool_destroy(&CTX_POOL(ctx));
}

int clsr_repl(void) {
  Stack stack = {};
  Context ctx = {};
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
      Node *eval_status = eval_program(CTX_PARSE_ROOT(&ctx), &ctx);

      if (eval_status != const_false) {
        // obj_fprintf(stdout, CTX_PEEK(&ctx)), printf("\n");
        ;
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
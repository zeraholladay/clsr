#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

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

jmp_buf eval_error_jmp;

void clsr_init(Context *ctx) {
  static int sym_save_bool = 0;

  if (!sym_save_bool && (sym_save_bool = 1))
    sym_save_init();

  CTX_POOL(ctx) = pool_init(OBJ_POOL_CAPACITY, sizeof(Node));
  CTX_ENV(ctx) = env_new(NULL);
  reset_parse_context(ctx);
}

void clsr_destroy(Context *ctx) {
  reset_parse_context(ctx);
  free(CTX_ENV(ctx)), CTX_ENV(ctx) = NULL;
  pool_destroy(&CTX_POOL(ctx));
}

static void clsr_eval_program(Context *ctx) {
  if (setjmp(eval_error_jmp) == 0) {

    Node *eval_result = eval_program(CTX_PARSE_ROOT(ctx), ctx);
    Node *node = _str(eval_result, ctx);
    printf("%s\n", get_string(node));
    free(node->as.string); // FIXME
  }
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
      break; // TODO: Something on error
    }

    yyin = fmemopen((void *)full_input, len, "r");

    reset_parse_context(&ctx);
    int parse_status = yyparse(&ctx);

    yylex_destroy();
    fclose(yyin);

    if (parse_status) {
      fprintf(stderr, "Parse failed\n");
      continue; // TODO: syntax error
    }

    clsr_eval_program(&ctx);
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
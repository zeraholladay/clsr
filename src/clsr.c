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
extern int yyparse(ParseContext *ctx);
extern void yylex_destroy(void);

void clsr_init(Stack *stack, ParseContext *parser_ctx, EvalContext *eval_ctx) {
  parser_ctx->obj_pool = obj_pool_init(OBJ_POOL_CAPACITY);
  reset_parse_context(parser_ctx);

  STACK_INIT(stack);

  eval_ctx->stack = stack;
  eval_ctx->env = env_new(NULL);
}

void clsr_destroy(Stack *stack, ParseContext *parser_ctx,
                  EvalContext *eval_ctx) {
  reset_parse_context(parser_ctx);
  obj_pool_destroy(&(parser_ctx->obj_pool));

  STACK_FREE(stack);

  eval_ctx->stack = NULL;
  FREE(eval_ctx->env);
}

int clsr_repl(void) {
  Stack stack = {};
  ParseContext parser_ctx = {};
  EvalContext eval_ctx = {};

  clsr_init(&stack, &parser_ctx, &eval_ctx);

  rl_init();

  char full_input[REPL_BUF_SIZ];

  for (;;) {
    int len = rl_readline(full_input, sizeof(full_input));

    if (len < 0) {
      continue; // TODO: Something
    }

    yyin = fmemopen((void *)full_input, len, "r");

    reset_parse_context(&parser_ctx);
    int parse_status = yyparse(&parser_ctx);

    yylex_destroy();
    fclose(yyin);

    if (parse_status == 0) {
      Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);

      if (eval_status == obj_true) {
        printf("=>TRUE\n");
      } else {
        printf("=>FALSE\n");
      }

    } else {
      fprintf(stderr, "Parse failed\n");
      continue; // TODO: syntax error
    }
  }
}

#else

extern int clsr_repl(void);

int main(void) {
#if YYDEBUG
  yydebug = YYDEBUG;
#endif
  clsr_repl();
}

#endif
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clsr.h"
#include "common.h"
#include "parser.h"

#ifndef REPL_MAIN

#ifndef REPL_BUF_SIZ
#define REPL_BUF_SIZ 8192
#endif

extern FILE *yyin;
extern int yyparse(ParseContext *ctx);
extern void yylex_destroy(void);

void clsr_init(Stack *stack, ParseContext *parser_ctx, EvalContext *eval_ctx) {
  obj_init_reserved_literals();

  reset_parse_context(parser_ctx);
  parser_ctx->obj_pool = obj_pool_init(OBJ_POOL_CAPACITY);

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

char **repl_attempted_completion_function(const char *text, int start,
                                          int end) {
  (void)text;
  (void)start;
  (void)end;

  rl_bind_key('\t', rl_insert);
  return NULL;
}

int repl_readline(char *full_input, size_t n) {
  size_t len = 0;
  char *line = NULL;

  full_input[0] = '\0';

  do {
    if (line) {
      free(line);
      line = NULL;
    }

    line = readline(len == 0 ? "clsr> " : "... ");

    if (!line)
      return -1;

    size_t line_len = strlen(line);

    if (len + line_len + 2 >= n) {
      free(line);
      return -1;
    }

    if (len > 0) {
      full_input[len++] = '\n';
    }

    strcpy(&full_input[len], line);
    len += line_len;

  } while (len == 0 || full_input[len - 1] != '\n');

  if (full_input[0]) {
    add_history(full_input);
  }

  free(line);
  return len;
}

int repl(void) {
  Stack stack = {};
  ParseContext parser_ctx = {};
  EvalContext eval_ctx = {};

  clsr_init(&stack, &parser_ctx, &eval_ctx);

  rl_attempted_completion_function = repl_attempted_completion_function;

  char full_input[REPL_BUF_SIZ];

  for (;;) {
    int len = repl_readline(full_input, sizeof(full_input));

    if (len < 0) {
      continue; // TODO: Something
    }

    yyin = fmemopen((void *)full_input, len, "r");

    int parse_status = yyparse(&parser_ctx);

    yylex_destroy();
    fclose(yyin);

    if (parse_status == 0) {
      Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);

      if (eval_status != obj_false) {
        printf("=>TRUE\n");
        Obj *obj_peek = PEEK((&eval_ctx)->stack);
        obj_fprintf(stdout, obj_peek);
        printf("\n");
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

extern int repl(void);

int main(void) { repl(); }

#endif
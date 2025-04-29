%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "clsr.h"
#include "parser.h"

#define yyerror(ctx, s)      \
  do {                       \
    yyerror_handler(ctx, s); \
    YYABORT;                 \
  } while (0)

int yylex(void);
void yyerror_handler(ParseContext *ctx, const char *s);

extern int yylineno;
%}

%code requires {
#include "clsr.h"

typedef struct ParseContext {
    ObjPool *obj_pool;
    Obj *root_obj;
    ObjPoolWrapper *parse_mark;
} ParseContext;

void reset_parse_context(ParseContext *ctx);
}

%parse-param {ParseContext *ctx}

%union {
    int num;
    const char *sym;
    const struct PrimOp *prim;
    struct Obj *obj;
}

%type <prim> nullary_prim_op
%type <prim> nary_prim_op
%type <obj> input expression expressions args arg

%define api.token.prefix {TOK_}

%token ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL
%token <prim> APPLY CLOSURE IF LOOKUP PUSH RETURN SET

%%

input:
    expressions {
        ctx->root_obj = $1;
        YYACCEPT;
    }
    | error {
        yyerror(ctx, "Parse error\n");
    }
;

expressions:
    /* empty */ {
        $$ = obj_new_empty_expr_list(ctx->obj_pool);
    }
    | expressions expression {
        $$ = obj_expr_list_append($1, $2);
    }
;

expression:
    nullary_prim_op {
        $$ = obj_new_call(ctx->obj_pool, $1, NULL);
    }
    | nary_prim_op '(' args ')' {
        $$ = obj_new_call(ctx->obj_pool, $1, $3);
      }
    | CLOSURE '(' args ')' '(' expressions ')' {
        $$ = obj_new_closure(ctx->obj_pool, $3, $6);
    }
    | IF '(' expressions ')' '(' expressions ')' {
        $$ = obj_new_if(ctx->obj_pool, $3, $6);
    }
;

nullary_prim_op:
      APPLY
    | LOOKUP
    | RETURN
    | SET
;

nary_prim_op:
      PUSH
;

args:
      /* empty */ {
        $$ = obj_new_empty_expr_list(ctx->obj_pool);
      }
    | args arg {
        $$ = obj_expr_list_append($1, $2);
      }
;

arg:
      INT_LITERAL {
        $$ = obj_new_literal_int(ctx->obj_pool, $1);
      }
    | SYM_LITERAL {
        $$ = obj_new_literal_sym(ctx->obj_pool, $1);
      }
;

%%

void reset_parse_context(ParseContext *ctx) {
    assert(ctx);
    assert(ctx->obj_pool);

    // no pool. assumes it has already been allocated.
    ctx->root_obj = NULL;
    ctx->parse_mark = ctx->obj_pool->free_list;
}

void yyerror_handler(ParseContext *ctx, const char *s) {
    fprintf(stderr, "Syntax error: line %d: %s\n", yylineno, s);
    obj_pool_reset_from_mark(ctx->obj_pool, ctx->parse_mark);
    reset_parse_context(ctx);
}

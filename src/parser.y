%{
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "clsr.h"
#include "parser.h"

#define yyerror(ctx, s)      \
  do {                       \
    yyerror_handler(ctx, s); \
    YYABORT;                 \
  } while (0)

int yylex(ClsrContext *ctx);
void yyerror_handler(ClsrContext *ctx, const char *s);

extern int yylineno;
%}

%code requires {
#include <stddef.h>

#include "clsr.h"
#include "rb_tree.h"

const char *sym_intern(rb_node **root, const char *s, size_t n);
void reset_parse_context(ClsrContext *ctx);
}

%lex-param   {ClsrContext *ctx}
%parse-param {ClsrContext *ctx}

%union {
    int num;
    const char *sym;
    const struct PrimOp *prim;
    struct Obj *obj;
}

%type <prim> nullary
%type <prim> nary
%type <obj> input expression expressions args arg literal_keyword

%define api.token.prefix {TOK_}

%token ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL
%token <prim> APPLY CLOSURE FALSE IF LOOKUP PUSH RETURN SET TRUE

%%

input:
    expressions {
        ctx->parser_ctx.root_obj = $1;
        YYACCEPT;
    }
    | expressions error {
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
    nullary {
        $$ = obj_new_call(ctx->obj_pool, $1, NULL);
    }
    | nary '(' args ')' {
        $$ = obj_new_call(ctx->obj_pool, $1, $3);
      }
    | CLOSURE '(' args ')' '(' expressions ')' {
        $$ = obj_new_closure(ctx->obj_pool, $3, $6);
    }
    | IF '(' expressions ')' '(' expressions ')' {
        $$ = obj_new_if(ctx->obj_pool, $3, $6);
    }
    | literal_keyword {
        $$ = $1;
    }
;

nullary:
    APPLY
    | LOOKUP
    | RETURN
    | SET
;

nary:
      PUSH
;

literal_keyword:
    TRUE {
        $$ = obj_true;
    }
    | FALSE {
        $$ = obj_false;
    }

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
    | literal_keyword {
        $$ = $1;
    }
;

%%

const char *sym_intern(rb_node **root, const char *s, size_t n) {
  rb_node *node = rb_lookup(*root, s, n);

  if (node)
    return RB_KEY(node);

  node = rb_alloc();

  if (!node)
    die(LOCATION);

  RB_KEY(node) = safe_strndup(s, n);
  RB_KEY_LEN(node) = n;
  // note: no RB_VAL here. ie symbols don't have values.

  if (!RB_KEY(node))
    die(LOCATION);

  rb_insert(root, node);

  return RB_KEY(node);
}

void reset_parse_context(ClsrContext *ctx) {
    assert(ctx);
    assert(ctx->obj_pool);

    /* no ctx->pool. assumes it has already been allocated. */
    ctx->parser_ctx.root_obj = NULL;
    ctx->parser_ctx.parse_mark = ctx->obj_pool->free_list;
}

void yyerror_handler(ClsrContext *ctx, const char *s) {
    fprintf(stderr, "Syntax error: line %d: %s\n", yylineno, s);
    obj_pool_reset_from_mark(ctx->obj_pool, ctx->parser_ctx.parse_mark);
    reset_parse_context(ctx);
}

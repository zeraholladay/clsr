%{
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

int yylex(ParseContext *ctx);
void yyerror_handler(ParseContext *ctx, const char *s);

extern int yylineno;
%}

%code requires {
#include "clsr.h"

typedef struct ParseContext {
    ObjPool *obj_pool;
    Obj *root_obj;
    struct {
        unsigned int parens;
        unsigned int eof;
    } lexer_state;
} ParseContext;

void reset_parse_context(ParseContext *ctx);
}

%lex-param   {ParseContext *ctx}
%parse-param {ParseContext *ctx}

%union {
    int num;
    const char *sym;
    const struct PrimOp *prim;
    struct Obj *obj;
}

%type <prim> nullary_prim_op
%type <prim> nary_prim_op
%type <obj> input expression expressions args arg expression_with_nl

%define api.token.prefix {TOK_}

%token ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL
%token <prim> APPLY CLOSURE LOOKUP PUSH RETURN SET

%%

input:
    '\n' {
        ctx->root_obj = NULL;
        YYACCEPT;
    }
  | expressions {
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
  | expressions expression_with_nl {
        $$ = obj_expr_list_append($1, $2);
    }
;

expression_with_nl:
    expression '\n' {
        $$ = $1;
    }
;

expression:
    nullary_prim_op {
        $$ = obj_new_call(ctx->obj_pool, $1, NULL);
    }
  | nary_prim_op args {
        $$ = obj_new_call(ctx->obj_pool, $1, $2);
    }
  | CLOSURE args '(' opt_nl expressions opt_nl ')' {
        $$ = obj_new_closure(ctx->obj_pool, $2, $5);
    }
;

opt_nl:
    /* empty */
  | '\n'
;

nullary_prim_op:
    APPLY
    | LOOKUP
    | RETURN
    | SET {
        $$ = $1;
    }
;

nary_prim_op:
    PUSH {
        $$ = $1;
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
;

%%

void reset_parse_context(ParseContext *ctx) {
    // TODO: free any nodes pointed to *root_obj
    ctx->root_obj = NULL;
    ctx->lexer_state.parens = 0;
    ctx->lexer_state.eof = 0;
}

void yyerror_handler(ParseContext *ctx, const char *s) {
    fprintf(stderr, "Syntax error: line %d: %s\n", yylineno, s);
    reset_parse_context(ctx);
}

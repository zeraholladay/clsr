%{
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "obj.h"
#include "parser.h"
#include "prim_op.h"

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
#include "obj.h"

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
%type <obj> input expression expressions args arg

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
    | expressions {  // no more '\n'
        ctx->root_obj = $1;
        YYACCEPT;
    }
    | error {
        yyerror(ctx, "Some text here\n");
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
    nullary_prim_op '\n' {
        DEBUG("[YACC] nullary_prim_op\n");
        $$ = obj_new_call(ctx->obj_pool, $1, NULL);
    }
    | nary_prim_op args '\n' {
        DEBUG("[YACC] nary_prim_op\n");
        $$ = obj_new_call(ctx->obj_pool, $1, $2);
    }
    | CLOSURE args '(' expressions ')' '\n' {
        DEBUG("[YACC] CLOSURE\n");
        $$ = obj_new_closure(ctx->obj_pool, $2, $4);
    }
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
        DEBUG("[YACC] INT_LITERAL %d (line %d)\n", $1, yylineno);
        $$ = obj_new_literal_int(ctx->obj_pool, $1);
    }
    | SYM_LITERAL {
        DEBUG("[YACC] SYM_LITERAL %s (line %d)\n", $1, yylineno);
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

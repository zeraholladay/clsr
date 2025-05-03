%{
#include <assert.h>
#include <stdio.h>

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
#include "clsr.h"

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

%token <prim> TRUE FALSE
%token <prim> APPLY EQ IS LOOKUP PUSH RETURN SET
%token <prim> CLOSURE IF
%token <prim> ADD SUB MUL DIV

%%

input:
    expressions {
        CTX_PARSE_ROOT(ctx) = $1;
        YYACCEPT;
    }
    | expressions error {
        yyerror(ctx, "Parse error\n");
    }
;

expressions:
    /* empty */ {
        $$ = obj_new_empty_expr_list(CTX_POOL(ctx));
    }
    | expressions expression {
        $$ = obj_expr_list_append($1, $2);
    }
;

expression:
    nullary {
        $$ = obj_new_call(CTX_POOL(ctx), $1, NULL);
    }
    | nary '(' args ')' {
        $$ = obj_new_call(CTX_POOL(ctx), $1, $3);
      }
    | CLOSURE '(' args ')' '(' expressions ')' {
        $$ = obj_new_closure(CTX_POOL(ctx), $3, $6);
    }
    | IF '(' expressions ')' '(' expressions ')' {
        $$ = obj_new_if(CTX_POOL(ctx), $3, $6);
    }
    | literal_keyword {
        $$ = $1;
    }
;

nullary:
    APPLY
    | EQ
    | IS
    | LOOKUP
    | RETURN
    | SET
    | ADD
    | SUB
    | MUL
    | DIV
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
        $$ = obj_new_empty_expr_list(CTX_POOL(ctx));
      }
    | args arg {
        $$ = obj_expr_list_append($1, $2);
    }
;

arg:
    INT_LITERAL {
        $$ = obj_new_literal_int(CTX_POOL(ctx), $1);
    }
    | SYM_LITERAL {
        $$ = obj_new_literal_sym(CTX_POOL(ctx), $1);
    }
    | literal_keyword {
        $$ = $1;
    }
;

%%

void reset_parse_context(ClsrContext *ctx) {
    assert(ctx);
    assert(CTX_POOL(ctx));

    /* assumes pool has already been allocated. */
    CTX_PARSE_ROOT(ctx) = NULL;
    CTX_PARSE_MARK(ctx) = CTX_POOL(ctx)->free_list;
}

void yyerror_handler(ClsrContext *ctx, const char *s) {
    fprintf(stderr, "Syntax error: line %d: %s\n", yylineno, s);
    pool_reset_from_mark(CTX_POOL(ctx), CTX_PARSE_MARK(ctx));
    reset_parse_context(ctx);
}

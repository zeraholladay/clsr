// clang-format off
%{
// clang-format off
#include <stdio.h>

#include "core_def.h"
#include "eval.h"
#include "parser.h"

#define yyerror(ctx, s)                                                        \
  do {                                                                         \
    yyerror_handler(ctx, s);                                                   \
    YYABORT;                                                                   \
} while (0)

int yylex(Context * ctx);
void yyerror_handler(Context * ctx, const char *s);

extern int yylineno;
// clang-format off
%}

%code requires {
#include "core_def.h"

void reset_parse_context(Context *ctx);
}

%lex-param   {Context *ctx}
%parse-param {Context *ctx}

%union {
    Integer integer;
    const char *symbol;
    const struct PrimitiveFn *prim_fn;
    struct Node *node;
}

%type <node> program exprs expr list_expr list_form literal_expr
%type <node> lambda_form param_form

%token ERROR LAMBDA QUOTE
%token <prim_fn> PRIMITIVE
%token <integer> INTEGER
%token <symbol>  SYMBOL

%%

program
    : exprs {
        CTX_PARSE_ROOT(ctx) = $1;
        YYACCEPT;
    }
    | exprs error {
        CTX_PARSE_ROOT(ctx) = NULL;
        yyerror(ctx, "Parse error\n");
        YYABORT;
    }
    ;

exprs
    : /* empty */ {
        $$ = CONS(NULL, NULL, ctx);
    }
    | expr exprs {
        $$ = CONS($1, $2, ctx);
    }
    ;

expr
    : list_expr
    | literal_expr
    | QUOTE expr {
        Node *quote = cons_primfn(CTX_POOL(ctx), PRIM_FN(QUOTE));
        $$ = LIST2(quote, $2, ctx);
    }
    ;

list_expr
    : '(' ')' {
        $$ = EMPTY_LIST(ctx);
    }
    | '(' list_form ')' {
        $$ = $2;
    }
    | '(' lambda_form ')' {
        $$ = LIST1($2, ctx);
    }
    ;

literal_expr
    : SYMBOL {
        $$ = cons_symbol(CTX_POOL(ctx), $1);
    }
    | PRIMITIVE {
        $$ = cons_primfn(CTX_POOL(ctx), $1);
    }
    | INTEGER {
        $$ = cons_integer(CTX_POOL(ctx), $1);
    }
    ;

list_form
    : expr {
        $$ = LIST1($1, ctx);
    }
    | expr list_form {
        $$ = CONS($1, $2, ctx);
    }
    ;

lambda_form
    : LAMBDA '(' param_form ')' exprs {
        $$ = cons_lambda(CTX_POOL(ctx), $3, $5, CTX_ENV(ctx));
    }
    ;

param_form
    : /* empty */ {
        $$ = CONS(NULL, NULL, ctx);
    }
    | SYMBOL param_form {
        Node *sym_node = cons_symbol(CTX_POOL(ctx), $1);
        $$ = CONS(sym_node, $2, ctx);
    }
    ;

%%
// clang-format off

void reset_parse_context(Context *ctx) {
  /* assumes pool has already been allocated. */
  CTX_PARSE_ROOT(ctx) = NULL;
  CTX_PARSE_MARK(ctx) = CTX_POOL(ctx)->free_list;
}

void yyerror_handler(Context *ctx, const char *s) {
  fprintf(stderr, "Syntax error: line %d: %s\n", yylineno, s);
  pool_reset_from_mark(CTX_POOL(ctx), CTX_PARSE_MARK(ctx));
  reset_parse_context(ctx);
}

// clang-format off
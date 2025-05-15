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

%type <node> program expressions expression expression_list list literal_expr symbol
%type <node> lambda parameter_list

%token ERROR LAMBDA QUOTE
%token <prim_fn> PRIMITIVE
%token <integer> INTEGER
%token <symbol>  SYMBOL

%%

program
    : expressions {
        CTX_PARSE_ROOT(ctx) = $1;
        YYACCEPT;
    }
    | expressions error {
        CTX_PARSE_ROOT(ctx) = NULL;
        yyerror(ctx, "Parse error\n");
        YYABORT;
    }
    ;

expressions
    : /* empty */ {
        $$ = CONS(NULL, NULL, ctx);
    }
    | expression expressions {
        $$ = CONS($1, $2, ctx);
    }
    ;

expression
    : list
    | lambda
    | literal_expr
    | QUOTE expression {
        Node *quote = cons_primfn(CTX_POOL(ctx), PRIM_FN(QUOTE));
        Node *fn_args = CONS($2, CONS(NULL, NULL, ctx), ctx);
        $$ = CONS(quote, fn_args, ctx);
    }
    ;

list
    : '(' ')' {
        $$ = CONS(NULL, NULL, ctx);
    }
    | '(' expression_list ')' {
        $$ = $2;
    }
    ;

expression_list
    : expression {
        $$ = CONS($1, CONS(NULL, NULL, ctx), ctx);
    }
    | expression expression_list {
        $$ = CONS($1, $2, ctx);
    }
    ;

literal_expr
    : symbol
    | PRIMITIVE {
        $$ = cons_primfn(CTX_POOL(ctx), $1);
    }
    | INTEGER {
        $$ = cons_integer(CTX_POOL(ctx), $1);
    }
    ;

symbol
    : SYMBOL {
        $$ = cons_symbol(CTX_POOL(ctx), $1);
    }

lambda
    : LAMBDA '(' parameter_list ')' '(' program ')' {
        $$ = cons_lambda(CTX_POOL(ctx), $3, $6);
    }
    ;

parameter_list
    : /* empty */ {
        $$ = CONS(NULL, NULL, ctx);
    }
    symbol parameter_list {
        $$ = LIST(cons_symbol(CTX_POOL(ctx), $1), $2, ctx);
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
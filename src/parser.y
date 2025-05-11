%{
#include <assert.h>
#include <stdio.h>

#include "core_def.h"
#include "eval.h"
#include "parser.h"

#define PRIM_OP(name) prim_op_lookup(#name, sizeof(#name) - 1)

#define yyerror(ctx, s)      \
  do {                       \
    yyerror_handler(ctx, s); \
    YYABORT;                 \
  } while (0)

int yylex(Context *ctx);
void yyerror_handler(Context *ctx, const char *s);

extern int yylineno;
%}

%code requires {
#include "core_def.h"

void reset_parse_context(Context *ctx);
}

%lex-param   {Context *ctx}
%parse-param {Context *ctx}

%union {
    CLSR_INTEGER_TYPE integer;
    const char *symbol;
    const struct PrimOp *prim_op;
    struct Node *node;
}

%type <node> program expressions expression expression_list list number symbol 

%token QUOTE ERROR
%token <prim_op> PRIM_OP
%token <integer> INTEGER
%token <symbol> SYMBOL

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
        $$ = empty_list(ctx);
    }
    | expression expressions {
        $$ = cons($1, $2, ctx);
    }
    ;

expression
    : number                    
    | symbol                    
    | list
    | QUOTE expression {
        Node *quote = cons_primop(CTX_POOL(ctx), PRIM_OP(QUOTE));
        Node *fn_args = cons($2, empty_list(ctx), ctx);
        $$ = cons(quote, fn_args, ctx);
    }
    ;

list
    : '(' ')' {
        $$ = empty_list(ctx);
    }
    | '(' expression_list ')' {
        $$ = $2;
    }
    ;

expression_list
    : expression {
        $$ = cons($1, empty_list(ctx), ctx);
    }
    | expression expression_list {
        $$ = cons($1, $2, ctx);
    }

    ;

symbol
    : PRIM_OP {
        $$ = cons_primop(CTX_POOL(ctx), $1);
    }
    | SYMBOL {
        $$ = cons_symbol(CTX_POOL(ctx), $1);
    }
    ;

number
    : INTEGER {
        $$ = cons_integer(CTX_POOL(ctx), $1);
    }
    ;

%%

void reset_parse_context(Context *ctx) {
    assert(ctx);
    assert(CTX_POOL(ctx));

    /* assumes pool has already been allocated. */
    CTX_PARSE_ROOT(ctx) = NULL;
    CTX_PARSE_MARK(ctx) = CTX_POOL(ctx)->free_list;
}

void yyerror_handler(Context *ctx, const char *s) {
    fprintf(stderr, "Syntax error: line %d: %s\n", yylineno, s);
    pool_reset_from_mark(CTX_POOL(ctx), CTX_PARSE_MARK(ctx));
    reset_parse_context(ctx);
}

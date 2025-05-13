%{
#include <assert.h>
#include <stdio.h>

#include "core_def.h"
#include "eval.h"
#include "parser.h"

#define PRIMITIVE(name) primitive_lookup(#name, sizeof(#name) - 1)

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
    const struct Primitive *prim_op;
    struct Node *node;
}

%type <node> program expressions expression expression_list list number symbol 

%token QUOTE ERROR
%token <prim_op> PRIMITIVE
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
        $$ = CONS(NULL, NULL, ctx);
    }
    | expression expressions {
        $$ = CONS($1, $2, ctx);
    }
    ;

expression
    : number                    
    | symbol                    
    | list
    | QUOTE expression {
        Node *quote = cons_primop(CTX_POOL(ctx), PRIMITIVE(QUOTE));
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

symbol
    : PRIMITIVE {
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

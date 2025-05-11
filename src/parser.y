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
%define api.token.prefix {TOK_}

%lex-param   {Context *ctx}
%parse-param {Context *ctx}

%union {
    CLSR_INTEGER_TYPE num;
    const char *sym;
    const struct PrimOp *prim;
    struct Node *node;
}

%type <node> program expressions expression expression_list list number symbol 
%type <prim> primitive

%token QUOTE ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL

%token <prim> ADD APPLY CLOSURE CONS DIV EQ EVAL FIRST IF IS LEN LOOKUP MUL PAIR PUSH REST REPR RETURN SET STR SUB

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
    : primitive {
        $$ = cons_primop(CTX_POOL(ctx), $1);
    }
    | SYM_LITERAL {
        $$ = cons_symbol(CTX_POOL(ctx), $1);
    }
    ;

primitive
    : ADD
    | APPLY
    | CONS
    | CLOSURE
    | DIV
    | EQ
    | EVAL
    | FIRST
    | IF
    | IS
    | LOOKUP
    | LEN
    | MUL
    | PUSH
    | PAIR
    | REPR
    | REST
    | RETURN
    | SET
    | STR
    | SUB
    ;

number
    : INT_LITERAL {
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

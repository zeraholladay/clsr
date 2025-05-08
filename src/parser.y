%{
#include <assert.h>
#include <stdio.h>

#include "core_def.h"
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
    int num;
    const char *sym;
    const struct PrimOp *prim;
    struct Node *node;
}

%type <node> program expressions expression expression_list list number symbol 
%type <prim> primitive

%token QUOTE ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL

%token <prim> ADD APPLY CLOSURE CONS DIV EQ EVAL FIRST IF IS LEN LOOKUP MUL PUSH REST RETURN SET SUB

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
        $$ = cons_list(CTX_POOL(ctx), NULL, NULL);
    }
    | expressions expression {
        $$ = cons_list(CTX_POOL(ctx), $2, $1);
    }
    ;

expression
    : number                    
    | symbol                    
    | list
    | QUOTE expression {
        Node *quote = cons_c_fn(CTX_POOL(ctx), PRIM_OP(QUOTE));
        Node *fn_args = cons_list(CTX_POOL(ctx), $2,
                                  cons_list(CTX_POOL(ctx), NULL, NULL));
        $$ = cons_list(CTX_POOL(ctx), quote, fn_args);
    }
    ;

list
    : '(' ')' {
        $$ = cons_list(CTX_POOL(ctx), NULL, NULL);
    }
    | '(' expression_list ')' {
        $$ = $2;
    }
    ;

expression_list
    : expression {
        Node *empty = cons_list(CTX_POOL(ctx), NULL, NULL);
        $$ = cons_list(CTX_POOL(ctx), $1, empty);
    }
    | expression expression_list {
        $$ = cons_list(CTX_POOL(ctx), $1, $2);
    }

    ;

symbol
    : primitive {
        $$ = cons_c_fn(CTX_POOL(ctx), $1);
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
    | REST
    | RETURN
    | SET
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

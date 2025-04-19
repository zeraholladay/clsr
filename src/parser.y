%{
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "prim_op.h"
#include "mach.h"
#include "object.h"
#include "stack.h"

void yyerror(const char *s);
int yylex(void);

extern int yylineno;
extern Stack STACK;
%}

%union {
    int num;
    const char *sym;
    const struct prim_op *prim;
}

%type <prim> nullary_prim_op
%type <prim> nary_prim_op

%define api.token.prefix {TOK_}
%token ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL
%token <prim> APPLY CLOSURE LOOKUP PUSH RETURN SET

%%

program:
    expressions
;

expressions:
        /* empty */
    | expressions expression
;

expression:
    ERROR {
        // PRINT_ERROR("[YACC] syntax error on line %d\n", yylineno);
        YYACCEPT;
    }
    | nullary_prim_op '\n' {
        DEBUG("[YACC] nullary_prim_op \n");
    }
    | nary_prim_op args '\n' {
        DEBUG("[YACC] nary_prim_op\n");
    }
    | CLOSURE args '(' expressions ')' '\n' {
        DEBUG("[YACC] CLOSURE\n");
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
      /* empty */
    | args arg
;

arg:
    INT_LITERAL {
        DEBUG("[YACC PUSHING INT_LITERAL] %d (line %d)\n", $1, yylineno);
        PUSH(&STACK, $1);
    }
    | SYM_LITERAL {
        int addr;
        Object *obj = ALLOC_SYM(&addr);  //TO DO: fix me: error handling
        obj->as.symbol = $1;
        DEBUG("[YACC PUSHING SYM_LITERAL] '%s':%d (line %d)\n", $1, addr, yylineno);
        PUSH(&STACK, addr);
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[YACC yyerror] line %d: %s\n", yylineno, s);
}
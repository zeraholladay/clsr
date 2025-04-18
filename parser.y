%{
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "operator.h"
#include "mach.h"
#include "object.h"
#include "stack.h"

void yyerror(const char *s);
int yylex(void);

extern int yylineno;
%}

%union {
    int num;
    const char *sym;
    const struct op *op_ptr;
}

%type <op_ptr> operator

%token ERROR HALT NL
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL
%token <op_ptr> APPLY CLOSURE LOOKUP PUSH RETURN SET

%%

program:
    instructions
;

instructions:
        /* empty */
    | instructions instruction
;

instruction:
    ERROR {
        printf("syntax error on line %d\n", yylineno);
    }
    HALT {
        YYACCEPT;  // causes yyparse() to return 0
    }
    | operator args NL {
        DEBUG("[YACC] run_operator\n");
        run_operator($1);
    }
;

operator:
    APPLY
    | CLOSURE
    | LOOKUP
    | PUSH
    | RETURN
    | SET {
        $$ = $1;
    }
;

args:
      /* empty */
    | arg_list
;

arg_list:
      arg
    | arg_list arg
;

arg:
    INT_LITERAL {
        DEBUG("[YACC PUSHING INT_LITERAL] %d (line %d)\n", $1, yylineno);
        push($1);
    }
    | SYM_LITERAL {
        int addr;
        object_t *obj = ALLOC_SYM(&addr);  //TO DO: fix me: error handling
        obj->symbol = $1;
        DEBUG("[YACC PUSHING SYM_LITERAL] '%s':%d (line %d)\n", $1, addr, yylineno);
        push(addr);
    }
;

%%

void yyerror(const char *s) {
    ERRMSG("[YYERROR] line %d: %s\n", yylineno, s);
}
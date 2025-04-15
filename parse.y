%{
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "op.h"
#include "mach.h"

void yyerror(const char *s);
int yylex(void);

extern int yylineno;
%}

%union {
    int num;
    const struct op *op_ptr;
}

%type <op_ptr> op

%token ERROR HALT EOL
%token <num> INT_LITERAL
%token <op_ptr> CALL APPLY CLOSURE RETURN PUSH ADD SUB MUL DIV

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
    HALT EOL {
        YYACCEPT;  // causes yyparse() to return 0
    }
    | op args EOL {
        run_operator($1);
    }
;

op:
    PUSH
    | ADD
    | SUB
    | MUL
    | DIV {
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
        DEBUG("[PUSHING INT_LITERAL] %d (line %d)\n", $1, yylineno);
        push($1);  // or push_to(caller_fp) if needed
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[YYERROR] line %d: %s\n", yylineno, s);
}

int main(void) {
    yyparse();
    printf("Final stack: ");
    /* for (int i = 0; i < sp; ++i) {
        printf("%d ", stack[i]);
    } */
    printf("\n");
    return 0;
}

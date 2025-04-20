%{
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "common.h"
#include "prim_op.h"

void yyerror(const char *s);
int yylex(void);

extern int yylineno;
%}

%union {
    int num;
    const char *sym;
    const struct PrimOp *prim;
    struct ASTNode *ast;
}

%type <prim> nullary_prim_op
%type <prim> nary_prim_op
%type <ast> expression expressions args arg

%define api.token.prefix {TOK_}
%token ERROR
%token <num> INT_LITERAL
%token <sym> SYM_LITERAL
%token <prim> APPLY CLOSURE LOOKUP PUSH RETURN SET

%%

expressions:
    /* empty */ {
        $$ = ast_new_empty_expr_list();
    }
  | expressions expression {
        $$ = ast_expr_list_append($1, $2);
    }
;

expression:
    ERROR {
        // Not really returning an AST node — caller will handle failure.
        // PRINT_ERROR("[YACC] syntax error on line %d\n", yylineno);
        $$ = 0;
        YYACCEPT;
    }
    | nullary_prim_op '\n' {
        DEBUG("[YACC] nullary_prim_op\n");
        $$ = ast_new_call($1, NULL);
    }
    | nary_prim_op args '\n' {
        DEBUG("[YACC] nary_prim_op\n");
        $$ = ast_new_call($1, $2);
    }
    | CLOSURE args '(' expressions ')' '\n' {
        DEBUG("[YACC] CLOSURE\n");
        $$ = ast_new_closure($2, $4);
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
      /* empty */ {
        $$ = ast_new_empty_expr_list();
      }
    | args arg {
        $$ = ast_expr_list_append($1, $2);
    }
;

arg:
    INT_LITERAL {
        DEBUG("[YACC INT_LITERAL] %d (line %d)\n", $1, yylineno);
        $$ = ast_new_literal_int($1);
    }
    | SYM_LITERAL {
        DEBUG("[YACC SYM_LITERAL] %s (line %d)\n", $1, yylineno);
        $$ = ast_new_literal_sym($1);
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[YACC yyerror] line %d: %s\n", yylineno, s);
}
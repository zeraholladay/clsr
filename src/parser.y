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
        /* empty */
    | expressions expression
;

expression:
    ERROR {
        // Not really returning an AST node — caller will handle failure.
        // PRINT_ERROR("[YACC] syntax error on line %d\n", yylineno);
        $$ = 0;
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
      /* empty */ {
        //$$ = ast_new_expr_list(NULL, 0);
        ASTNode *n = NULL;
        if (ALLOC(n)) die(LOCATION);
        n->type = AST_List;
        n->as.list.nodes = NULL;
        n->as.list.count++;
        $$ = n;
      }
    | args arg {
        //$$ = ast_expr_list_append($1, $2);

        ASTNode *n1 = $1;
        ASTNode *n2 = $2;

        size_t count = n1->as.list.count;

        ASTNode **new_items = NULL;
        if (REALLOC_N(new_items, count + 1)) die(LOCATION);

        new_items[count] = n2;

        n1->as.list.nodes = new_items;
        n1->as.list.count++;

        $$ = n1;
}
;

arg:
    INT_LITERAL {
        ASTNode *n = NULL;
        if (ALLOC(n)) die(LOCATION);
        n->type = AST_Literal;
        n->as.literal.type = Literal_Int;
        n->as.literal.as.integer = $1;
        $$ = n;
    }
    | SYM_LITERAL {
        ASTNode *n = NULL;
        if (ALLOC(n)) die(LOCATION);
        n->type = AST_Literal;
        n->as.literal.type = Literal_Sym;
        n->as.literal.as.symbol = $1;
        $$ = n;
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[YACC yyerror] line %d: %s\n", yylineno, s);
}
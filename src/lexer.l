%option noyywrap
%option yylineno
%option noinput
%option nounput

%{
#include <stdio.h>
#include <stdlib.h>

#include "clsr.h"
#include "parser.h"
#include "common.h"

%}

NL       \r?\n
WS       [ \t]+
NUM      -?[0-9]+
ID       [a-zA-Z_][a-zA-Z0-9_]*

%%

<INITIAL>{
    "("             { return '('; }
    ")"             { return ')'; }
    {NUM}           { yylval.num = atoi(yytext); return TOK_INT_LITERAL; }
    {ID}            {
                    const PrimOp *p = prim_op_lookup(yytext, strlen(yytext));
                    if (p) {
                        yylval.prim = p;
                        return p->tok;
                    } 
                    yylval.sym = str_intern(yytext, yyleng);
                    return TOK_SYM_LITERAL;
                    }
    {WS}            { /* ignore */ }
    {NL}            { /* ignore */ }
    ;[^\n]*         { /* ignore */ }
    .               { return TOK_ERROR; }
}

%%

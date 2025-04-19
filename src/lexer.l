%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "prim_op.h"
#include "common.h"

extern int yylineno;
%}

COMMENT ;[^\n]*

OP (APPLY|CLOSURE|LOOKUP|PUSH|RETURN|SET)
WS [ \t]+
NL \r?\n

%x ARG END_CLOSURE

%%

<INITIAL>{
    {COMMENT}       { /* skip comment */ }
    {WS}            { /* skip whitespace */ }
    {WS}*{NL}       { /* skip empty or whitespace-only line */ }

    {OP} {
        DEBUG("[LEX] prim_op matched: yytext = '%s'\n", yytext);
        const PrimOp *p = prim_op_lookup(yytext, strlen(yytext));
        if (p) {
            BEGIN(ARG);
            yylval.prim = p;
            DEBUG("[LEX] returning prim code = '%d'\n", p->tok);
            return p->tok;
        } else {
            DEBUG("[GPERF] prim not found = '%s'\n", yytext);
            return TOK_ERROR;  // fallback if not found
        }
    }

    \) {
        BEGIN(END_CLOSURE);
        return ')';
    }
}

<END_CLOSURE>{
    {COMMENT} { /* skip comment */ }
    {WS}      { /* skip whitespace */ }
    {NL} {
        BEGIN(INITIAL);
        return '\n';
    }
}

<ARG>{
    {COMMENT}       { /* skip comment */ }
    {WS}            { /* skip whitespace */ }
    \( {
        BEGIN(INITIAL);
        return '(';
    }
    -*[0-9]+ {
        yylval.num = atoi(yytext);
        return TOK_INT_LITERAL;
    }
    [a-zA-Z][a-zA-Z0-9]* {
        yylval.sym = str_intern(yytext);
        return TOK_SYM_LITERAL;
    }
    {NL} {
        BEGIN(INITIAL);
        return '\n';
    }
}

%%

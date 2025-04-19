%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "prim_op.h"
#include "log.h"
#include "strutil.h"

extern int yylineno;
%}

COMMENT ;[^\n]*

OP (APPLY|CLOSURE|LOOKUP|PUSH|RETURN|SET)
WS [ \t]+
NL \r?\n

%x ARG

%%

<INITIAL>{COMMENT}|{WS}|{WS}*{NL}  { /* skip comment, whitespace, or empty lines*/ }
<INITIAL>{OP} {
    DEBUG("[LEX] prim_op matched: yytext = '%s'\n", yytext);
    const prim_op *prim = prim_op_lookup(yytext, strlen(yytext));
    if (prim) {
        BEGIN(ARG);
        yylval.prim = prim;
        DEBUG("[LEX] returning prim code = '%d'\n", prim->code);
        return prim->code;
    } else {
        DEBUG("[GPERF] prim not found = '%s'\n", yytext);
        return ERROR;  // fallback if not found
    }
}

<ARG>{COMMENT}|{WS} { /* skip comment or whitespace */ }
<ARG>\( { return '('; }
<ARG>\) { return ')'; }
<ARG>-*[0-9]+ {
    yylval.num = atoi(yytext);
    return INT_LITERAL;
}
<ARG>[a-zA-Z][a-zA-Z0-9]* {
    yylval.sym = sym_intern(yytext, yyleng);
    return SYM_LITERAL;
}
<ARG>{NL} {
    BEGIN(INITIAL);
    return NL;
}

%%
